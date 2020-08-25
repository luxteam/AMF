#include "../../include/core/Factory.h"
#include "../../common/TraceAdapter.h"
#include "../../common/AMFFactory.h"
#include "../../include/core/Buffer.h"

#include <iostream>

#define AMF_FACILITY L"amf_metal_test1"

int main(int argc, char *argv[])
{
    printf("start");
	const char* kernel_src = R"(
		#include <metal_stdlib>

		using namespace metal;

		kernel void process_array(
            device const float*     inA,
            device const float*     inB,
		    device float*           result,
            constant int32_t &      value,
            constant float &        extra,

            uint2 global_id [[thread_position_in_grid]],
            uint2 local_id [[thread_position_in_threadgroup]],
            uint2 group_id [[threadgroup_position_in_grid]],
            uint2 group_size [[threads_per_threadgroup]],
            uint2 grid_size [[threads_per_grid]]
            )
		{
            if(!global_id.x)
            {
                result[global_id.x] = global_id.y;
            }
            else if(1 == global_id.x)
            {
                result[global_id.x] = global_id.y;
            }

            else if(2 == global_id.x)
            {
                result[global_id.x] = local_id.x;
            }
            else if(3 == global_id.x)
            {
                result[global_id.x] = local_id.y;
            }

            else if(4 == global_id.x)
            {
                result[global_id.x] = local_id.x;
            }
            else if(5 == global_id.x)
            {
                result[global_id.x] = local_id.y;
            }

            else if(6 == global_id.x)
            {
                result[global_id.x] = group_id.x;
            }
            else if(7 == global_id.x)
            {
                result[global_id.x] = group_id.y;
            }

            else if(8 == global_id.x)
            {
                result[global_id.x] = group_size.x;
            }
            else if(9 == global_id.x)
            {
                result[global_id.x] = group_size.y;
            }

            else if(10 == global_id.x)
            {
                result[global_id.x] = grid_size.x;
            }
            else if(11 == global_id.x)
            {
                result[global_id.x] = grid_size.y;
            }

            else
            {
                result[global_id.x] = inA[global_id.x] + inB[global_id.x] - value + extra;
            }
		}
        )";
    AMFFactoryHelper helper;
    helper.Init();
    amf::AMFFactory* factory = helper.GetFactory();

printf("0");
    amf::AMFContextPtr context;
    factory->CreateContext(&context);
	//context1->SetProperty(AMF_CONTEXT_DEVICE_TYPE, AMF_CONTEXT_DEVICE_TYPE_GPU);
    amf::AMFComputeFactoryPtr metalComputeFactory;
    context->GetMetalComputeFactory(&metalComputeFactory);
    int deviceCount = metalComputeFactory->GetDeviceCount();
printf("\nDevice count :%d\n", deviceCount);
    if (deviceCount < 1)
        return -1;

    amf::AMFComputeDevicePtr pComputeDevice;
    metalComputeFactory->GetDeviceAt(0, &pComputeDevice);

    context->InitMetalEx(pComputeDevice.GetPtr());

    amf::AMFPrograms* pPrograms;
    factory->GetPrograms(&pPrograms);

    amf::AMF_KERNEL_ID kernel = 0;
    pPrograms->RegisterKernelSource(&kernel, L"kernelIDName", "process_array", strlen(kernel_src), (amf_uint8*)kernel_src, "option");

	g_AMFFactory.Init();
    g_AMFFactory.GetDebug()->AssertsEnable(true);
    AMF_RESULT res;
printf("1");
    //amf::AMFComputeDevicePtr pComputeDevice;

    amf::AMFComputePtr pCompute;
    //pComputeDevice->CreateCompute(nullptr, &pCompute);
    res =  context->GetCompute(amf::AMF_MEMORY_METAL, &pCompute);
printf("%d", (int)res);
    amf::AMFComputeKernelPtr pKernel;
    res = pCompute->GetKernel(kernel, &pKernel);

    AMF_RETURN_IF_FALSE(res == AMF_OK, -1);

    amf::AMFBufferPtr input1;
    amf::AMFBufferPtr input2;
    amf::AMFBufferPtr output;

    const int arraysSize = 128;

    AMF_RETURN_IF_FALSE(AMF_OK == context->AllocBuffer(amf::AMF_MEMORY_HOST, arraysSize * sizeof(float), &input1), -1);
    AMF_RETURN_IF_FALSE(AMF_OK == context->AllocBuffer(amf::AMF_MEMORY_HOST, arraysSize * sizeof(float), &input2), -1);
    AMF_RETURN_IF_FALSE(AMF_OK == context->AllocBuffer(amf::AMF_MEMORY_METAL, arraysSize * sizeof(float), &output), -1);

    {
        float  *inputData = static_cast<float*>(input1->GetNative());
        for (int k = 0; k < arraysSize; k++)
        {
            inputData[k] = k;
        }
    }

    {
        float  *inputData = static_cast<float*>(input2->GetNative());
        for (int k = 0; k < arraysSize; k++)
        {
            inputData[k] = arraysSize - k;
        }
    }

    input1->Convert(amf::AMF_MEMORY_METAL);
    input2->Convert(amf::AMF_MEMORY_METAL);

    AMF_RETURN_IF_FALSE(AMF_OK == pKernel->SetArgBuffer(0, input1, amf::AMF_ARGUMENT_ACCESS_READ), -1);
    AMF_RETURN_IF_FALSE(AMF_OK == pKernel->SetArgBuffer(1, input2, amf::AMF_ARGUMENT_ACCESS_READ), -1);
    AMF_RETURN_IF_FALSE(AMF_OK == pKernel->SetArgBuffer(2, output, amf::AMF_ARGUMENT_ACCESS_WRITE), -1);
    AMF_RETURN_IF_FALSE(AMF_OK == pKernel->SetArgInt32(3, -1), -1);
    AMF_RETURN_IF_FALSE(AMF_OK == pKernel->SetArgFloat(4, 0.345f), -1);

    amf_size sizeLocal[3] = {arraysSize, 1, 1};
    amf_size sizeGlobal[3] = {arraysSize, 1, 1};
    amf_size offset[3] = {0, 0, 0};

    pKernel->GetCompileWorkgroupSize(sizeLocal);
    pKernel->Enqueue(1, offset, sizeGlobal, sizeLocal);
    pCompute->FlushQueue();

    //output->Convert(amf::AMF_MEMORY_HOST);
    amf::AMFBuffer *subBuffer = NULL;
    output->CreateSubBuffer(&subBuffer, 0, arraysSize * sizeof(float));
    subBuffer->Convert(amf::AMF_MEMORY_HOST);

    float  *outputData = static_cast<float*>(subBuffer->GetNative());

    for (int k = 0; k < arraysSize; k++ )
    {
        std::cout << "result[" << k << "] = " << outputData[k] << std::endl;
    }

    return 0;
}
