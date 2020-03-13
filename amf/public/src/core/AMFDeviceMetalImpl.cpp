
#include "AMFDeviceMetalImpl.h"
#include "AMFComputeKernelMetal.h"
#include "ProgramsImpl.h"

AMFDeviceMetalImpl::AMFDeviceMetalImpl(AMFContextImpl *pContext, void * native)
: AMFDeviceImpl(AMF_MEMORY_METAL, 0, pContext),
    m_compute(NULL)
{
    m_device = new MetalDeviceWrapper(native);

    AMF_RESULT res = m_device->CreateCompute(&m_compute);
    assert(res == AMF_OK);
}

AMFDeviceMetalImpl::~AMFDeviceMetalImpl()
{
    delete m_device;
    delete m_compute;
}

AMF_RESULT AMFDeviceMetalImpl::AllocateBuffer(amf_size size, void **ppHandle)
{
    AMF_RETURN_IF_FALSE(size != 0, AMF_INVALID_ARG, L"AllocateBuffer() - size cannot be 0");
    AMF_RETURN_IF_FALSE(ppHandle != 0, AMF_INVALID_ARG, L"AllocateBuffer() - ppHandle cannot be 0");
    *ppHandle = m_device->AllocateBuffer(size);
    AMF_RETURN_IF_FALSE(*ppHandle != nullptr, AMF_OUT_OF_MEMORY, L"Error: Failed to allocate device memory!\n");

    return AMF_OK;
}

AMF_RESULT AMFDeviceMetalImpl::AttachBuffer(amf_size size, void *pHandle)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::ReleaseBuffer(void *pHandle, bool attached)
{
    if (!attached)
    {
        return m_device->ReleaseBuffer(pHandle);
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceMetalImpl::CreateSubBuffer(AMFBuffer* pHandle, void ** subBuffer, amf_size offset, amf_size size)
{
    return m_device->CreateSubBuffer(pHandle->GetNative(), subBuffer, offset, size);
}

AMF_RESULT AMFDeviceMetalImpl::CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size)
{
    return m_device->CopyBuffer(pDestHandle, dstOffset, pSourceHandle, srcOffset, size);
}

AMF_RESULT AMFDeviceMetalImpl::CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking)
{
    return m_device->CopyBufferToHost(pDest, pSourceHandle, srcOffset, size, blocking);
}

AMF_RESULT AMFDeviceMetalImpl::CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking)
{
    return m_device->CopyBufferFromHost(pDestHandle, dstOffset, pSource, size, blocking);
}

AMF_RESULT AMFDeviceMetalImpl::FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_MEMORY_TYPE AMFDeviceMetalImpl::GetMemoryType()
{
    return GetType();
}
void *AMFDeviceMetalImpl::GetNativeContext()
{
    return NULL;
}
void *AMFDeviceMetalImpl::GetNativeDeviceID()
{
    return NULL;
}

void *AMFDeviceMetalImpl::GetNativeCommandQueue()
{
    return NULL;
}

AMF_RESULT AMFDeviceMetalImpl::GetKernel(AMF_KERNEL_ID kernelID, AMFComputeKernel **kernel)
{
    AMFKernelStorage::KernelData *kernelData;
    AMFKernelStorage::Instance()->GetKernelData(&kernelData, kernelID);
    const char * source = (const char *)kernelData->data;

    MetalComputeKernelWrapper *pKernel = NULL;
    AMF_RESULT res = m_compute->GetKernel(source, &pKernel);
    AMF_RETURN_IF_FALSE(res == AMF_OK, AMF_INVALID_ARG, L"GetKernel");

    AMFComputeKernelMetal * computeKernel = new AMFComputeKernelMetal(kernelID, pKernel);
    *kernel = computeKernel;
    (*kernel)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFDeviceMetalImpl::PutSyncPoint(AMFComputeSyncPoint **ppSyncPoint)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::FinishQueue()
{
    return m_compute->FinishQueue();
}

AMF_RESULT AMFDeviceMetalImpl::FlushQueue()
{
    return m_compute->FlushQueue();
}

AMF_RESULT AMFDeviceMetalImpl::FillPlane(AMFPlane *pPlane, const amf_size origin[], const amf_size region[], const void *pColor)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::FillBuffer(AMFBuffer *pBuffer, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::ConvertPlaneToBuffer(AMFPlane *pSrcPlane, AMFBuffer **ppDstBuffer)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::CopyBuffer(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffset)
{
    return CopyBuffer(pDstBuffer->GetNative(), dstOffset, pSrcBuffer->GetNative(), srcOffset, size);
}

AMF_RESULT AMFDeviceMetalImpl::CopyPlane(AMFPlane *pSrcPlane, const amf_size srcOrigin[], const amf_size region[], AMFPlane *pDstPlane, const amf_size dstOrigin[])
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::CopyBufferToHost(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, void *pDest, amf_bool blocking)
{
    return CopyBufferToHost(pDest, pSrcBuffer->GetNative(), srcOffset, size, blocking);
}

AMF_RESULT AMFDeviceMetalImpl::CopyBufferFromHost(const void *pSource, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffsetInBytes, amf_bool blocking)
{
    return CopyBufferFromHost(pDstBuffer, dstOffsetInBytes, pSource, size, blocking);
}

AMF_RESULT AMFDeviceMetalImpl::CopyPlaneToHost(AMFPlane *pSrcPlane, const amf_size origin[], const amf_size region[], void *pDest, amf_size dstPitch, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::CopyPlaneFromHost(void *pSource, const amf_size origin[], const amf_size region[], amf_size srcPitch, AMFPlane *pDstPlane, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceMetalImpl::ConvertPlaneToPlane(AMFPlane *pSrcPlane, AMFPlane **ppDstPlane, AMF_CHANNEL_ORDER order, AMF_CHANNEL_TYPE type)
{
    return AMF_NOT_IMPLEMENTED;
}


AMF_RESULT AMFDeviceMetalImpl::CreateCompute(void *reserved, AMFCompute **ppCompute)
{
    AMFDeviceMetalImpl *computeOCL = new AMFDeviceMetalImpl(m_pContext, m_device->GetNativeDevice());
    *ppCompute = computeOCL;
    (*ppCompute)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFDeviceMetalImpl::CreateComputeEx(void *pCommandQueue, AMFCompute **ppCompute)
{
    AMFDeviceMetalImpl *computeOCL = new AMFDeviceMetalImpl(m_pContext, m_device->GetNativeDevice());
    *ppCompute = computeOCL;
    (*ppCompute)->Acquire();
    return AMF_OK;
}

void *AMFDeviceMetalImpl::GetNativePlatform()
{
    return NULL;
}