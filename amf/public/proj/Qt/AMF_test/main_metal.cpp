#include "../../../include/core/Factory.h"
#include "../../../common/AMFFactory.h"
#include "../../../include/core/Buffer.h"
#include "iostream"

#define CL_TARGET_OPENCL_VERSION 120
using namespace std;

int main(int argc, char *argv[])
{
    AMFFactoryHelper helper;
    helper.Init();
    amf::AMFFactory* factory = helper.GetFactory();

    amf::AMFContextPtr context;
    factory->CreateContext(&context);

	//context1->SetProperty(AMF_CONTEXT_DEVICE_TYPE, AMF_CONTEXT_DEVICE_TYPE_GPU);
    context->InitMetal();

    amf::AMFPrograms* pPrograms;
    factory->GetPrograms(&pPrograms);

    amf::AMF_KERNEL_ID kernel = 0;
    const char* kernel_src = "process_array";
    pPrograms->RegisterKernelSource(&kernel, L"kernelIDName", "process_array", strlen(kernel_src), (amf_uint8*)kernel_src, "option");

	g_AMFFactory.Init();
    g_AMFFactory.GetDebug()->AssertsEnable(true);
    AMF_RESULT res;

    //amf::AMFComputeDevicePtr pComputeDevice;

    amf::AMFComputePtr pCompute;
    //pComputeDevice->CreateCompute(nullptr, &pCompute);
    context->GetCompute(amf::AMF_MEMORY_METAL, &pCompute);

    amf::AMFComputeKernelPtr pKernel;
    res = pCompute->GetKernel(kernel, &pKernel);

    amf::AMFBuffer *input = NULL;
    amf::AMFBuffer *output = NULL;

    res = context->AllocBuffer(amf::AMF_MEMORY_HOST, 1024 * sizeof(float), &input);
    res = context->AllocBuffer(amf::AMF_MEMORY_METAL, 1024 * sizeof(float), &output);

    float  *inputData = static_cast<float*>(input->GetNative());
    for (int k = 0; k < 1024; k++)
    {
        inputData[k] = rand() / 50.00;;
    }

    input->Convert(amf::AMF_MEMORY_METAL);

    res = pKernel->SetArgBuffer(0, input, amf::AMF_ARGUMENT_ACCESS_READ);
    res = pKernel->SetArgBuffer(1, output, amf::AMF_ARGUMENT_ACCESS_WRITE);

    amf_size sizeLocal[3] = {1024, 1, 1};
    amf_size sizeGlobal[3] = {1024, 1, 1};
    amf_size offset[3] = {0, 0, 0};

    pKernel->GetCompileWorkgroupSize(sizeLocal);

    pKernel->Enqueue(1, offset, sizeGlobal, sizeLocal);
    pCompute->FlushQueue();
    pCompute->FinishQueue();

    //output->Convert(amf::AMF_MEMORY_HOST);
    amf::AMFBuffer *subBuffer = NULL;
    output->CreateSubBuffer(&subBuffer, 0, 1024 * sizeof(float));
    subBuffer->Convert(amf::AMF_MEMORY_HOST);

    float  *outputData = static_cast<float*>(subBuffer->GetNative());

    for (int k = 0; k < 10; k++ )
    {
        printf("result[%d] = %f \n", k, outputData[k]);
    }

    return 0;
}
