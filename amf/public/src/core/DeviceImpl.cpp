#include "DeviceImpl.h"

AMFDeviceImpl::AMFDeviceImpl(AMF_MEMORY_TYPE type, amf_size cacheSize, AMFContextImpl* pContext)
    :m_type(type),
     m_cacheSize(cacheSize),
     m_pContext(pContext)
{
}


AMF_MEMORY_TYPE AMFDeviceImpl::GetType()
{
    return m_type;
}

AMF_RESULT AMFDeviceImpl::AllocateBuffer(amf_size size, void **ppHandle)
{
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFDeviceImpl::AttachBuffer(amf_size size, void *pHandle)
{
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFDeviceImpl::ReleaseBuffer(void *pHandle, bool attached)
{
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFDeviceImpl::CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size)
{
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFDeviceImpl::CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking)
{
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFDeviceImpl::CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking)
{
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFDeviceImpl::FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    return AMF_NOT_SUPPORTED;
}

