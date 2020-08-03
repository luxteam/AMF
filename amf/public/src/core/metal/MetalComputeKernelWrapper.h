#pragma once
#include "../../../include/core/Result.h"
class MetalComputeKernel;

class MetalComputeKernelWrapper
{
public:
    MetalComputeKernelWrapper(void * computeKernel);

    AMF_RESULT SetArgBuffer(void * buffer, int index);

    AMF_RESULT GetCompileWorkgroupSize(amf_size workgroupSize[3]);

    AMF_RESULT Enqueue(amf_size globalSize[3], amf_size localSize[3]);
private:
    MetalComputeKernel * m_kernel;
};