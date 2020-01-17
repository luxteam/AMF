#include "../../../include/core/Factory.h"
#include "../../../common/AMFFactory.h"
#include "../../../include/core/Buffer.h"

#define CL_TARGET_OPENCL_VERSION 120

int main(int argc, char *argv[])
{
    AMFFactoryHelper helper;
    helper.Init();
    amf::AMFFactory* factory = helper.GetFactory();

    amf::AMFContextPtr context;
    factory->CreateContext(&context);
    context->InitOpenCL();
    amf::AMFComputeFactoryPtr oclComputeFactory;
    context->GetOpenCLComputeFactory(&oclComputeFactory);

    amf::AMFPrograms* pPrograms;
    factory->GetPrograms(&pPrograms);

    amf::AMF_KERNEL_ID kernel = 0;
    const char* kernel_src = "\n" \
                             "__kernel void square( __global float* input, __global float* output, \n" \
                             " const unsigned int count) {            \n" \
                             " int i = get_global_id(0);              \n" \
                             " if(i < count) \n" \
                             " output[i] = -input[i]; \n" \
                             "}                     \n";
    pPrograms->RegisterKernelSource(&kernel, L"kernelIDName", "square", strlen(kernel_src), (amf_uint8*)kernel_src, "option");

    int deviceCount = oclComputeFactory->GetDeviceCount();
	g_AMFFactory.Init();
    g_AMFFactory.GetDebug()->AssertsEnable(true);
	g_AMFFactory.GetTrace()->EnableWriter(AMF_TRACE_WRITER_FILE, true);
	g_AMFFactory.GetTrace()->SetWriterLevel(AMF_TRACE_WRITER_FILE, AMF_TRACE_TEST);

    for(int i = 0; i < deviceCount; ++i)
    {
        AMF_RESULT res;
        amf::AMFComputeDevicePtr pComputeDevice;
        oclComputeFactory->GetDeviceAt(i, &pComputeDevice);
        pComputeDevice->GetNativeContext();

		

        amf::AMFComputePtr pCompute;
        pComputeDevice->CreateCompute(nullptr, &pCompute);

        amf::AMFComputeKernelPtr pKernel;
        res = pCompute->GetKernel(kernel, &pKernel);

        amf::AMFBuffer *input = NULL;
        amf::AMFBuffer *output = NULL;

        res = context->AllocBuffer(amf::AMF_MEMORY_HOST, 1024, &input);
        float  *inputData = static_cast<float*>(input->GetNative());
        for (int k = 0; k < 1024; k++)
        {
            inputData[k] = rand() / 50.00;;
        }

        input->Convert(amf::AMF_MEMORY_OPENCL);


        res = context->AllocBuffer(amf::AMF_MEMORY_OPENCL, 1024, &output);

        res = pKernel->SetArgBuffer(1, output, amf::AMF_ARGUMENT_ACCESS_WRITE);
        res = pKernel->SetArgBuffer(0, input, amf::AMF_ARGUMENT_ACCESS_READ);
        res = pKernel->SetArgInt32(2, 1024);

        amf_size sizeLocal[3] = {1024, 0, 0};
        amf_size sizeGlobal[3] = {1024, 0, 0};
        amf_size offset[3] = {0, 0, 0};

        pKernel->GetCompileWorkgroupSize(sizeLocal);

        pKernel->Enqueue(1, offset, sizeGlobal, sizeLocal);
        pCompute->FlushQueue();
        pCompute->FinishQueue();
        output->Convert(amf::AMF_MEMORY_HOST);
        float  *outputData = static_cast<float*>(output->GetNative());
        for (int k = 0; k < 1024; k++ )
        {
            printf("result[%d] = %f ", k, outputData[k]);
        }
    }

    return 0;
}
