#include "../../include/core/Factory.h"
#include "../../common/TraceAdapter.h"
#include "../../common/AMFFactory.h"
#include "../../include/core/Buffer.h"

#include <iostream>

#define AMF_FACILITY L"flush_queue_issue"

int main(int argc, char *argv[])
{
    g_AMFFactory.Init();

    AMFFactoryHelper helper;
    helper.Init();
    helper.GetDebug()->AssertsEnable(true);

    amf::AMFFactory* factory = helper.GetFactory();

    amf::AMFContextPtr context;
    AMF_RETURN_IF_FAILED(factory->CreateContext(&context));

    AMF_RETURN_IF_FAILED(context->SetProperty(AMF_CONTEXT_DEVICE_TYPE, AMF_CONTEXT_DEVICE_TYPE_GPU));

    amf::AMFComputeFactoryPtr metalComputeFactory;
    AMF_RETURN_IF_FAILED(context->GetMetalComputeFactory(&metalComputeFactory));

    int deviceCount = metalComputeFactory->GetDeviceCount();
    std::cout << "Device count: " << deviceCount << std::endl;

    if(deviceCount < 1)
    {
        return -1;
    }

    amf::AMFComputeDevicePtr pComputeDevice;
    AMF_RETURN_IF_FAILED(metalComputeFactory->GetDeviceAt(1, &pComputeDevice));

    amf::AMFVariant name;
    AMF_RETURN_IF_FAILED(pComputeDevice->GetProperty(AMF_DEVICE_NAME, &name));
    std::cout << name.ToString().c_str() << std::endl;

    AMF_RETURN_IF_FAILED(context->InitMetalEx(pComputeDevice.GetPtr()));

    amf::AMFPrograms *pPrograms;
    AMF_RETURN_IF_FAILED(factory->GetPrograms(&pPrograms));

    const char* init_src = R"(
        #include <metal_stdlib>

        using namespace metal;

        kernel void init_array(
            device int32_t *        result,
            constant int32_t &      value,

            uint2 global_id [[thread_position_in_grid]]
            )
        {
            result[global_id.x] = value;
        }
        )";

    const char* add_src = R"(
        #include <metal_stdlib>

        using namespace metal;

        kernel void add_array(
            device int32_t *        result,
            constant int32_t &      value,

            uint2 global_id [[thread_position_in_grid]]
            )
        {
            result[global_id.x] += value;
        }
        )";

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
        }
        )";

    amf::AMF_KERNEL_ID kernel1_1(0);
    AMF_RETURN_IF_FAILED(pPrograms->RegisterKernelSource(&kernel1_1, L"init_src", "init_array", strlen(init_src), (amf_uint8*)init_src, ""));

    amf::AMF_KERNEL_ID kernel1_2(0);
    AMF_RETURN_IF_FAILED(pPrograms->RegisterKernelSource(&kernel1_2, L"add_src", "add_array", strlen(init_src), (amf_uint8*)add_src, "option"));

    amf::AMF_KERNEL_ID kernel12_1;
    AMF_RETURN_IF_FAILED(pPrograms->RegisterKernelSource(&kernel12_1, L"mult_src", "mult_array", strlen(mult_src), (amf_uint8*)mult_src, "option"));

    amf::AMFComputePtr pCompute1;
    AMF_RETURN_IF_FAILED(pComputeDevice->CreateCompute(nullptr, &pCompute1));

    amf::AMFComputePtr pCompute2;
    //AMF_RETURN_IF_FAILED(pComputeDevice->CreateCompute(nullptr, &pCompute2));

    amf::AMFComputeKernelPtr pKernel1;
    AMF_RETURN_IF_FAILED(pCompute1->GetKernel(kernel1_1, &pKernel1));

    amf::AMFComputeKernelPtr pKernel2;
    AMF_RETURN_IF_FAILED(pCompute1->GetKernel(kernel1_2, &pKernel2));

    //amf::AMFComputeKernelPtr pKernel3;
    //AMF_RETURN_IF_FAILED(pCompute2->GetKernel(kernel12_1, &pKernel3));
    
    amf::AMFComputeKernelPtr pKernel4;
    AMF_RETURN_IF_FAILED(pCompute1->GetKernel(kernel1_1, &pKernel4));

    const int arraysSize = 128;

    amf::AMFBufferPtr buf1;
    AMF_RETURN_IF_FAILED(context->AllocBuffer(amf::AMF_MEMORY_METAL, arraysSize * sizeof(int32_t), &buf1));

    for(int cycle(0); cycle < 10000000000; ++cycle)
    {
        AMF_RETURN_IF_FAILED(pKernel1->SetArgBuffer(0, buf1, amf::AMF_ARGUMENT_ACCESS_WRITE));
        AMF_RETURN_IF_FAILED(pKernel1->SetArgInt32(1, cycle));

        AMF_RETURN_IF_FAILED(pKernel2->SetArgBuffer(0, buf1, amf::AMF_ARGUMENT_ACCESS_WRITE));
        AMF_RETURN_IF_FAILED(pKernel2->SetArgInt32(1, cycle));
        
        AMF_RETURN_IF_FAILED(pKernel4->SetArgBuffer(0, buf1, amf::AMF_ARGUMENT_ACCESS_WRITE));
        AMF_RETURN_IF_FAILED(pKernel4->SetArgInt32(1, cycle));

        amf_size sizeLocal[3] = {arraysSize, 1, 1};
        amf_size sizeGlobal[3] = {arraysSize, 1, 1};

        //std::cout << "cycle " << cycle << std::endl;

        //enqueue will do the following:
        //[MTLComputeCommandEncoder
        //    dispatchThreads:workgroupSize
        //    threadsPerThreadgroup:sizeInWorkgroup];
        //[MTLComputeCommandEncoder endEncoding];
        AMF_RETURN_IF_FAILED(pKernel1->Enqueue(1, 0, sizeGlobal, sizeLocal));
        AMF_RETURN_IF_FAILED(pKernel2->Enqueue(1, 0, sizeGlobal, sizeLocal));
        AMF_RETURN_IF_FAILED(pKernel4->Enqueue(1, 0, sizeGlobal, sizeLocal));
        
        //continue;
        
        //flush queue will do the following
        //for (id<MTLCommandBuffer> buffer in buffers)
        //{
        //    [buffer commit];
        //}
        AMF_RETURN_IF_FAILED(pCompute1->FlushQueue());
        //for (id<MTLCommandBuffer> buffer in m_kernelBuffers)
        //{
        //    [buffer waitUntilCompleted];
        //}
        AMF_RETURN_IF_FAILED(pCompute1->FinishQueue());
        
        /*
        AMF_RETURN_IF_FAILED(buf1->Convert(amf::AMF_MEMORY_HOST));

        {
            int32_t *outputData = static_cast<int32_t*>(buf1->GetNative());

            for (int k = 0; k < arraysSize; k++ )
            {
                std::cout << "result[" << k << "] = " << outputData[k] << std::endl;
            }
        }
        
        AMF_RETURN_IF_FAILED(buf1->Convert(amf::AMF_MEMORY_METAL));
        */
        
        //AMF_RETURN_IF_FALSE(AMF_OK == pKernel3->SetArgBuffer(0, buf1, amf::AMF_ARGUMENT_ACCESS_WRITE), -1);
        //AMF_RETURN_IF_FALSE(AMF_OK == pKernel3->SetArgInt32(1, cycle), -1);

        //AMF_RETURN_IF_FALSE(AMF_OK == pKernel3->Enqueue(1, 0, sizeGlobal, sizeLocal), -1);
        //AMF_RETURN_IF_FALSE(AMF_OK == pCompute2->FlushQueue(), -1);
        //AMF_RETURN_IF_FALSE(AMF_OK == pCompute2->FinishQueue(), -1);
    }

    AMF_RETURN_IF_FAILED(buf1->Convert(amf::AMF_MEMORY_HOST));

    {
        float  *outputData = static_cast<float*>(buf1->GetNative());

        for (int k = 0; k < arraysSize; k++ )
        {
            std::cout << "result[" << k << "] = " << outputData[k] << std::endl;
        }
    }

    return AMF_OK;
}
