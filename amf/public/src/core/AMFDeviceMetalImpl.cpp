
#include "AMFDeviceMetalImpl.h"
#include "AMFComputeKernelMetal.h"
#include "ProgramsImpl.h"

AMFComputeDeviceMetalImpl::AMFComputeDeviceMetalImpl(
    AMFContextImpl *    pContext,
    AMFDeviceImpl *     device,
    void *              native,
    const std::string & name
    )
    :  m_pContext(pContext), m_deviceImpl(device)
{
    m_device = new MetalDeviceWrapper(native);

    SetProperty(AMF_DEVICE_NAME, AMFVariant(name.c_str()));
}

AMFComputeDeviceMetalImpl::~AMFComputeDeviceMetalImpl()
{
    delete m_device;
}

void *AMFComputeDeviceMetalImpl::GetNativePlatform()
{
    return nullptr;
}

void *AMFComputeDeviceMetalImpl::GetNativeDeviceID()
{
    return m_device->GetNativeDevice();
}

void *AMFComputeDeviceMetalImpl::GetNativeContext()
{
    return nullptr;
}

AMF_RESULT AMFComputeDeviceMetalImpl::CreateCompute(void *reserved, AMFCompute **ppCompute)
{
    AMFVariant name;
    AMF_RETURN_IF_FAILED(GetProperty(AMF_DEVICE_NAME, &name));

    AMFComputeMetalImpl* computeOCL = new AMFComputeMetalImpl(
        m_pContext,
        m_device->GetNativeDevice(),
        name.ToString().c_str()
        );
    *ppCompute = computeOCL;
    (*ppCompute)->Acquire();

    return AMF_OK;
}

AMF_RESULT AMFComputeDeviceMetalImpl::CreateComputeEx(void *pCommandQueue, AMFCompute **ppCompute)
{
    //TODO: native commandqueue support
    return CreateCompute(nullptr, ppCompute);
}

AMFDeviceImpl *AMFComputeDeviceMetalImpl::GetDevice() const
{
    return m_deviceImpl;
}

MetalDeviceWrapper *AMFComputeDeviceMetalImpl::GetDeviceWrapper() const
{
    return m_device;
}

AMFDeviceMetalImpl::AMFDeviceMetalImpl(
    AMFContextImpl *                pContext,
    void *                          native,
    const std::string &             name
    )
    : AMFDeviceImpl(AMF_MEMORY_METAL, 0, pContext)
{
    AMFComputeDeviceMetalImpl * compDeviceImpl = new AMFComputeDeviceMetalImpl(
        m_pContext,
        this,
        native,
        name
        );
    m_device = compDeviceImpl->GetDeviceWrapper();
    m_computeDevice = compDeviceImpl;
}

AMFDeviceMetalImpl::~AMFDeviceMetalImpl()
{

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

AMF_RESULT AMFDeviceMetalImpl::CreateSubBuffer(AMFBuffer *pHandle, void **subBuffer, amf_size offset, amf_size size)
{
    return m_device->CreateSubBuffer(pHandle->GetNative(), subBuffer, offset, size);
}

AMF_RESULT AMFDeviceMetalImpl::MapToHost(AMFBuffer *pHandle, void **memory, amf_size offset, amf_size size, bool blocking)
{
    //TODO: implement
    return AMF_NOT_IMPLEMENTED;
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

void *AMFDeviceMetalImpl::GetNativeCommandQueue()
{
    return nullptr;
}

const AMFComputeDevicePtr &AMFDeviceMetalImpl::GetComputeDevice() const
{
    return m_computeDevice;
}

MetalDeviceWrapper *AMFDeviceMetalImpl::GetDeviceWrapper() const
{
    return m_device;
}

AMFComputeMetalImpl::AMFComputeMetalImpl(
    AMFContextImpl *    pContext,
    void *              native,
    const std::string & name
    )
{
    m_device.reset(new AMFDeviceMetalImpl(pContext, native, name));

    MetalComputeWrapper *metalComputeWrapper(nullptr);
    AMF_RESULT res = m_device->GetDeviceWrapper()->CreateCompute(&metalComputeWrapper);
    assert(res == AMF_OK && metalComputeWrapper);
    m_compute.reset(metalComputeWrapper);
}

AMFComputeMetalImpl::~AMFComputeMetalImpl()
{
}

AMF_MEMORY_TYPE AMFComputeMetalImpl::GetMemoryType()
{
    return m_device->GetType();
}

void *AMFComputeMetalImpl::GetNativeContext()
{
    return nullptr;
}

void *AMFComputeMetalImpl::GetNativeDeviceID()
{
    return nullptr;
}

void *AMFComputeMetalImpl::GetNativeCommandQueue()
{
    return nullptr;
}

AMF_RESULT AMFComputeMetalImpl::GetKernel(AMF_KERNEL_ID kernelID, AMFComputeKernel **kernel)
{
    AMFKernelStorage::KernelData *kernelData(nullptr);
    AMFKernelStorage::Instance()->GetKernelData(&kernelData, kernelID);

    const char * source = reinterpret_cast<const char *>(&kernelData->data.front());

    MetalComputeKernelWrapper *pKernel = NULL;
    AMF_RESULT res = m_compute->GetKernel(source, kernelData->kernelName.c_str(), &pKernel);
    AMF_RETURN_IF_FALSE(res == AMF_OK, AMF_INVALID_ARG, L"GetKernel");

    AMFComputeKernelMetal * computeKernel = new AMFComputeKernelMetal(kernelID, pKernel);
    *kernel = computeKernel;
    (*kernel)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFComputeMetalImpl::PutSyncPoint(AMFComputeSyncPoint **ppSyncPoint)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeMetalImpl::FinishQueue()
{
    return m_compute->FinishQueue();
}

AMF_RESULT AMFComputeMetalImpl::FlushQueue()
{
    return m_compute->FlushQueue();
}

AMF_RESULT AMFComputeMetalImpl::FillPlane(AMFPlane *pPlane, const amf_size origin[], const amf_size region[], const void *pColor)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeMetalImpl::FillBuffer(AMFBuffer *pBuffer, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeMetalImpl::ConvertPlaneToBuffer(AMFPlane *pSrcPlane, AMFBuffer **ppDstBuffer)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeMetalImpl::CopyBuffer(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffset)
{
    return m_device->CopyBuffer(pDstBuffer->GetNative(), dstOffset, pSrcBuffer->GetNative(), srcOffset, size);
}

AMF_RESULT AMFComputeMetalImpl::CopyPlane(AMFPlane *pSrcPlane, const amf_size srcOrigin[], const amf_size region[], AMFPlane *pDstPlane, const amf_size dstOrigin[])
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeMetalImpl::CopyBufferToHost(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, void *pDest, amf_bool blocking)
{
    return m_device->CopyBufferToHost(pDest, pSrcBuffer->GetNative(), srcOffset, size, blocking);
}

AMF_RESULT AMFComputeMetalImpl::CopyBufferFromHost(const void *pSource, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffsetInBytes, amf_bool blocking)
{
    return m_device->CopyBufferFromHost(pDstBuffer, dstOffsetInBytes, pSource, size, blocking);
}

AMF_RESULT AMFComputeMetalImpl::CopyPlaneToHost(AMFPlane *pSrcPlane, const amf_size origin[], const amf_size region[], void *pDest, amf_size dstPitch, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeMetalImpl::CopyPlaneFromHost(void *pSource, const amf_size origin[], const amf_size region[], amf_size srcPitch, AMFPlane *pDstPlane, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeMetalImpl::ConvertPlaneToPlane(AMFPlane *pSrcPlane, AMFPlane **ppDstPlane, AMF_CHANNEL_ORDER order, AMF_CHANNEL_TYPE type)
{
    return AMF_NOT_IMPLEMENTED;
}
