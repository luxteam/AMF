#include "DeviceHostImpl.h"

AMFDeviceHostImpl::AMFDeviceHostImpl(AMFContextImpl* pContext)
    :AMFDeviceImpl(AMF_MEMORY_HOST, 0, pContext)
{

}


AMF_MEMORY_TYPE AMFDeviceHostImpl::GetType()
{
    return AMF_MEMORY_HOST;
}

AMF_RESULT AMFDeviceHostImpl::AllocateBuffer(amf_size size, void **ppHandle)
{
    AMF_RETURN_IF_FALSE(size != 0, AMF_INVALID_ARG, L"AllocateBuffer() - size cannot be 0");
    AMF_RETURN_IF_FALSE(ppHandle != 0, AMF_INVALID_ARG, L"AllocateBuffer() - ppHandle cannot be 0");
    *ppHandle = amf_virtual_alloc(size);
    AMF_RETURN_IF_FALSE(*ppHandle != nullptr, AMF_OUT_OF_MEMORY, L"AllocateBuffer() - no memory");
    return AMF_OK;
}

AMF_RESULT AMFDeviceHostImpl::AttachBuffer(amf_size size, void *pHandle)
{
    return AMF_OK;
}

AMF_RESULT AMFDeviceHostImpl::ReleaseBuffer(void *pHandle, bool attached)
{
    if(!attached)
    {
        amf_virtual_free(pHandle);
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceHostImpl::CreateSubBuffer(AMFBuffer * pHandle, void ** subBuffer, amf_size offset, amf_size size)
{
	return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceHostImpl::MapToHost(AMFBuffer * pHandle, void ** memory, amf_size offset, amf_size size, bool blocking)
{
	return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceHostImpl::CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size)
{
    memcpy(static_cast<amf_uint8*>(pDestHandle) + dstOffset, static_cast<amf_uint8*>(pSourceHandle) + srcOffset, size);
    return AMF_OK;
}

AMF_RESULT AMFDeviceHostImpl::CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking)
{
    return CopyBuffer(pDest, 0, pSourceHandle, srcOffset, size);
}

AMF_RESULT AMFDeviceHostImpl::CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking)
{
    return CopyBuffer(pDestHandle, dstOffset, const_cast<void*>(pSource), 0, size);
}

AMF_RESULT AMFDeviceHostImpl::FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    return AMF_NOT_IMPLEMENTED;
}
