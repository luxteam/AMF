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
    {
        const char* init_src = R"(
        #include <metal_stdlib>

        using namespace metal;

        kernel void init_array(
            device float*           result,
            constant int32_t &      value,

            uint2 global_id [[thread_position_in_grid]]
            )
        {
            result[global_id.x] = value;
        })";
        
        const char* add_src = R"(
        #include <metal_stdlib>

        using namespace metal;

        kernel void add_array(
            device float*           result,
            constant int32_t &      value,

            uint2 global_id [[thread_position_in_grid]]
            )
        {
            result[global_id.x] += value;
        })";
        
        const char* mult_src = R"(
        #include <metal_stdlib>

        using namespace metal;

        kernel void mult_array(
            device float*           result,
            constant int32_t &      value,

            uint2 global_id [[thread_position_in_grid]]
            )
        {
            result[global_id.x] = result[global_id.x] * value;
        })";
        
        amf::AMF_KERNEL_ID kernel1_1;
        pPrograms->RegisterKernelSource(&kernel1_1, L"init_src", "init_array", strlen(init_src), (amf_uint8*)init_src, "option");
        amf::AMF_KERNEL_ID kernel1_2;
        pPrograms->RegisterKernelSource(&kernel1_2, L"add_src", "add_array", strlen(init_src), (amf_uint8*)add_src, "option");
        
        amf::AMF_KERNEL_ID kernel12_1;
        pPrograms->RegisterKernelSource(&kernel12_1, L"mult_src", "mult_array", strlen(mult_src), (amf_uint8*)mult_src, "option");
        
        amf::AMFComputePtr pCompute1;
        pComputeDevice->CreateCompute(nullptr, &pCompute1);
        amf::AMFComputePtr pCompute2;
        pComputeDevice->CreateCompute(nullptr, &pCompute2);
        
        amf::AMFComputeKernelPtr pKernel1;
        res = pCompute1->GetKernel(kernel1_1, &pKernel1);
        
        amf::AMFComputeKernelPtr pKernel2;
        res = pCompute1->GetKernel(kernel1_2, &pKernel2);
        
        amf::AMFComputeKernelPtr pKernel3;
        res = pCompute2->GetKernel(kernel12_1, &pKernel3);
        
        const int arraysSize = 128;
        amf::AMFBufferPtr buf1;
        AMF_RETURN_IF_FALSE(AMF_OK == context->AllocBuffer(amf::AMF_MEMORY_METAL, arraysSize * sizeof(float), &buf1), -1);
        
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel1->SetArgBuffer(0, buf1, amf::AMF_ARGUMENT_ACCESS_WRITE), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel1->SetArgInt32(1, 2), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel2->SetArgBuffer(0, buf1, amf::AMF_ARGUMENT_ACCESS_WRITE), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel2->SetArgInt32(1, 2), -1);
        
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel3->SetArgBuffer(0, buf1, amf::AMF_ARGUMENT_ACCESS_WRITE), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel3->SetArgInt32(1, 2), -1);
        
        amf_size sizeLocal[3] = {arraysSize, 1, 1};
        amf_size sizeGlobal[3] = {arraysSize, 1, 1};

        pKernel1->GetCompileWorkgroupSize(sizeLocal);
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel1->Enqueue(1, 0, sizeGlobal, sizeLocal), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel2->Enqueue(1, 0, sizeGlobal, sizeLocal), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pCompute1->FlushQueue(), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pCompute1->FinishQueue(), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pKernel3->Enqueue(1, 0, sizeGlobal, sizeLocal), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pCompute2->FlushQueue(), -1);
        AMF_RETURN_IF_FALSE(AMF_OK == pCompute2->FinishQueue(), -1);
        buf1->Convert(amf::AMF_MEMORY_HOST);
        float  *outputData = static_cast<float*>(buf1->GetNative());

        for (int k = 0; k < arraysSize; k++ )
        {
            std::cout << "result[" << k << "] = " << outputData[k] << std::endl;
        }

    }

    amf::AMFComputePtr pCompute;
    pComputeDevice->CreateCompute(nullptr, &pCompute);

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

    pKernel->GetCompileWorkgroupSize(sizeLocal);
    AMF_RETURN_IF_FALSE(AMF_OK == pKernel->Enqueue(1, 0, sizeGlobal, sizeLocal), -1);
    AMF_RETURN_IF_FALSE(AMF_OK == pCompute->FlushQueue(), -1);

    //output->Convert(amf::AMF_MEMORY_HOST);
    amf::AMFBuffer *subBuffer = NULL;
    output->CreateSubBuffer(&subBuffer, 0, arraysSize * sizeof(float));
    float  pattern[4] ={0.3, 0.4, 0.5, 0.6};
    subBuffer->Fill(24 * sizeof(float), (arraysSize - 24)  * sizeof(float), &pattern[0], 4 * sizeof(float));
    pCompute->FillBuffer(subBuffer, 24 * sizeof(float), (arraysSize - 24)  * sizeof(float), &pattern[0], 4 * sizeof(float));
    subBuffer->Convert(amf::AMF_MEMORY_HOST);

//    float  *outputData = static_cast<float*>(subBuffer->GetNative());
//
//    for (int k = 0; k < arraysSize; k++ )
//    {
//        std::cout << "result[" << k << "] = " << outputData[k] << std::endl;
//    }

    return 0;
}
