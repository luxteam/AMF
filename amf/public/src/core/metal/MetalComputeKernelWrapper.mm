#include "MetalComputeKernelWrapper.h"
#include "MetalComputeKernel.h"

MetalComputeKernelWrapper::MetalComputeKernelWrapper(void * computeKernel)
{
    m_kernel = static_cast<MetalComputeKernel*>(computeKernel);
    assert(m_kernel != nil);
}

AMF_RESULT MetalComputeKernelWrapper::SetArgBuffer(void * buffer, int index)
{
    return m_kernel->SetArgBuffer((id<MTLBuffer>)buffer, index);
}

AMF_RESULT MetalComputeKernelWrapper::SetArgInt32(int32_t value, int index)
{
    return m_kernel->SetArgInt32(value, index);
}

AMF_RESULT MetalComputeKernelWrapper::SetArgInt64(int64_t value, int index)
{
    return m_kernel->SetArgInt64(value, index);
}

AMF_RESULT MetalComputeKernelWrapper::SetArgFloat(float value, int index)
{
    return m_kernel->SetArgFloat(value, index);
}

AMF_RESULT MetalComputeKernelWrapper::GetCompileWorkgroupSize(amf_size workgroupSize[3])
{
    MTLSize size = m_kernel->GetCompileWorkgroupSize(MTLSizeMake(workgroupSize[0],
                                                                 workgroupSize[1],
                                                                 workgroupSize[2]));
    workgroupSize[0] = size.width;
    workgroupSize[1] = size.height;
    workgroupSize[2] = size.depth;

    return AMF_OK;
}

AMF_RESULT MetalComputeKernelWrapper::Enqueue(amf_size globalSize[3], amf_size localSize[3])
{
    MTLSize workgroupSize = MTLSizeMake(globalSize[0], globalSize[1], globalSize[2]);
    MTLSize sizeInWorkgroup = MTLSizeMake(localSize[0], localSize[1], localSize[2]);

    return m_kernel->Enqueue(workgroupSize, sizeInWorkgroup);
}
