#include "AMFComputeKernelMetal.h"
#include "MetalComputeKernel.h"

AMFComputeKernelMetal::AMFComputeKernelMetal(AMF_KERNEL_ID kernelID, void * kernel)
{
    m_kernel = static_cast<MetalComputeKernel*>(kernel);
}

AMFComputeKernelMetal::~AMFComputeKernelMetal()
{
    delete m_kernel;
}

void * AMFComputeKernelMetal::GetNative()
{
    return NULL;
}

const wchar_t * AMFComputeKernelMetal::GetIDName()
{
    return L"AMFComputeKernelMetal";
}

AMF_RESULT    AMFComputeKernelMetal::SetArgPlaneNative(amf_size index, void* pPlane, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT    AMFComputeKernelMetal::SetArgBufferNative(amf_size index, void* pBuffer, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT    AMFComputeKernelMetal::SetArgPlane(amf_size index, AMFPlane* pPlane, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT    AMFComputeKernelMetal::SetArgBuffer(amf_size index, AMFBuffer* pBuffer, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    id<MTLBuffer> native = (id<MTLBuffer>)pBuffer->GetNative();
    return m_kernel->SetArgBuffer(native, index);
}

AMF_RESULT    AMFComputeKernelMetal::SetArgInt32(amf_size index, amf_int32 data)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT    AMFComputeKernelMetal::SetArgInt64(amf_size index, amf_int64 data)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT    AMFComputeKernelMetal::SetArgFloat(amf_size index, amf_float data)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT    AMFComputeKernelMetal::SetArgBlob(amf_size index, amf_size dataSize, const void* pData)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT    AMFComputeKernelMetal::GetCompileWorkgroupSize(amf_size workgroupSize[3])
{
    MTLSize size = m_kernel->GetCompileWorkgroupSize(workgroupSize[0]);
    workgroupSize[0] = size.width;
    workgroupSize[1] = size.height;
    workgroupSize[2] = size.depth;

    return AMF_OK;
}

AMF_RESULT    AMFComputeKernelMetal::Enqueue(amf_size dimension, amf_size globalOffset[3], amf_size globalSize[3], amf_size localSize[3])
{
    MTLSize workgroupSize = MTLSizeMake(globalSize[0], globalSize[1], globalSize[2]);
    MTLSize sizeInWorkgroup = MTLSizeMake(localSize[0], localSize[1], localSize[2]);

    return m_kernel->Enqueue(workgroupSize, sizeInWorkgroup);
}