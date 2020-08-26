#include "MetalDeviceWrapper.h"
#include "MetalDevice.h"

MetalDeviceWrapper::MetalDeviceWrapper(void * native)
{
    if (!!native)
        m_device = new MetalDevice((id<MTLDevice>)native);
    else
        m_device = new MetalDevice();
}

void * MetalDeviceWrapper::AllocateBuffer(size_t size)
{
    return (void*)m_device->AllocateBuffer(size);
}

AMF_RESULT MetalDeviceWrapper::ReleaseBuffer(void * native)
{
    return m_device->ReleaseBuffer((id<MTLBuffer>)native);
}

AMF_RESULT MetalDeviceWrapper::CopyBuffer(void * pDestHandle, size_t dstOffset, void * pSourceHandle, size_t srcOffset, size_t size)
{
    return m_device->CopyBuffer((id<MTLBuffer>)pDestHandle, dstOffset, (id<MTLBuffer>)pSourceHandle, srcOffset, size);
}

AMF_RESULT MetalDeviceWrapper::CopyBufferToHost(void *pDest, void * pSourceHandle, size_t srcOffset, size_t size, bool blocking)
{
    return m_device->CopyBufferToHost(pDest, (id<MTLBuffer>)pSourceHandle, srcOffset, size, blocking);
}

AMF_RESULT MetalDeviceWrapper::CopyBufferFromHost(void * pDestHandle, size_t dstOffset, const void *pSource, size_t size, bool blocking)
{
    return m_device->CopyBufferFromHost((id<MTLBuffer>)pDestHandle, dstOffset, pSource, size, blocking);
}

AMF_RESULT MetalDeviceWrapper::CreateCompute(MetalComputeWrapper ** compute)
{
    MetalCompute * computeImpl = nil;
    AMF_RESULT res = m_device->CreateCompute(&computeImpl);
    if (res != AMF_OK)
        return res;

    (*compute) = new MetalComputeWrapper((void*)computeImpl);
    return AMF_OK;
}
AMF_RESULT MetalDeviceWrapper::CreateSubBuffer(void* pHandle, void ** subBuffer, amf_size offset, amf_size size)
{
    return m_device->CreateSubBuffer((id<MTLBuffer>)pHandle, subBuffer, offset, size);
}

AMF_RESULT MetalDeviceWrapper::FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    return m_device->FillBuffer((id<MTLBuffer>)pDestHandle, dstOffset, dstSize, pSourcePattern, patternSize);
}

void * MetalDeviceWrapper::GetNativeDevice()
{
    return m_device->GetNativeDevice();
}
