#include "ContextImpl.h"
#include "DeviceHostImpl.h"
#include "DeviceOCLImpl.h"

#if defined(__APPLE__) && defined(ENABLE_METAL)
    #include "AMFDeviceMetalImpl.h"
    #include "AMFComputeFactoryMetal.h"
#endif
#include "ComputeOCL.h"
#include "BufferImpl.h"

AMFContextImpl::AMFContextImpl()
{

}

AMFContextImpl::~AMFContextImpl()
{
    m_pDeviceHost = nullptr;
    m_pDeviceOCL = nullptr;
    m_pDeviceMetal = nullptr;
}


AMF_RESULT AMFContextImpl::Terminate()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::InitDX9(void *pDX9Device)
{
    return AMF_NOT_IMPLEMENTED;
}

void *AMFContextImpl::GetDX9Device(AMF_DX_VERSION dxVersionRequired)
{
    return NULL;
}

AMF_RESULT AMFContextImpl::LockDX9()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::UnlockDX9()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::InitDX11(void *pDX11Device, AMF_DX_VERSION dxVersionRequired)
{
    return AMF_NOT_IMPLEMENTED;
}

void *AMFContextImpl::GetDX11Device(AMF_DX_VERSION dxVersionRequired)
{
    return NULL;
}

AMF_RESULT AMFContextImpl::LockDX11()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::UnlockDX11()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::InitOpenCL(void *pCommandQueue)
{
    if (!pCommandQueue)
    {
        AMF_RESULT res;
        amf::AMFComputeFactoryPtr oclComputeFactory;
        res = GetOpenCLComputeFactory(&oclComputeFactory);
        if (res == AMF_OK && oclComputeFactory->GetDeviceCount() > 0)
        {
            AMFComputeDevice* pComputeDevice;
            res = oclComputeFactory->GetDeviceAt(0, &pComputeDevice);
            if (res == AMF_OK)
            {
                AMFComputeDeviceOCLImpl* deviceImpl = dynamic_cast<AMFComputeDeviceOCLImpl*>(pComputeDevice);

                AMFDeviceImpl* device = deviceImpl->GetDevice();
                if (device)
                {
                    device->Acquire();
                    m_pDeviceOCL = device;
                    return AMF_OK;
                }
                return AMF_FAIL;
            }
        }
        return res;
    }
    else
    {
        cl_context context = NULL;
        cl_device_id deviceId = NULL;
        cl_platform_id platformId = NULL;

        size_t resultsize;
        int err = clGetCommandQueueInfo(cl_command_queue(pCommandQueue), CL_QUEUE_CONTEXT, sizeof(cl_context), &context, &resultsize);
        err |= clGetCommandQueueInfo(cl_command_queue(pCommandQueue), CL_QUEUE_DEVICE, sizeof(cl_device_id), &deviceId, &resultsize);
        err |= clGetDeviceInfo(deviceId, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &platformId, &resultsize);

		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to clGetCommandQueueInfo!\n");
			return AMF_FAIL;
		}

        m_pDeviceOCL = new AMFDeviceOCLImpl(platformId, deviceId, this, context);
    }
	return AMF_OK;
}

AMF_RESULT AMFContextImpl::InitOpenCLEx(AMFComputeDevice *pDevice)
{
    AMFComputeDeviceOCLImpl* deviceImpl = dynamic_cast<AMFComputeDeviceOCLImpl*>(pDevice);

    AMFDeviceImpl* device = deviceImpl->GetDevice();
    if (device)
    {
        device->Acquire();
        m_pDeviceOCL = device;
        return AMF_OK;
    }

    return AMF_INVALID_ARG;
}

void *AMFContextImpl::GetOpenCLContext()
{
    AMFDeviceOCLImpl* deviceImpl = dynamic_cast<AMFDeviceOCLImpl*>(m_pDeviceOCL.GetPtr());

    return deviceImpl->GetComputeDevice()->GetNativeContext();
}

void *AMFContextImpl::GetOpenCLCommandQueue()
{
    AMFDeviceOCLImpl* deviceImpl = dynamic_cast<AMFDeviceOCLImpl*>(m_pDeviceOCL.GetPtr());

    return deviceImpl->GetNativeCommandQueue();
}

void *AMFContextImpl::GetOpenCLDeviceID()
{
    AMFDeviceOCLImpl* deviceImpl = dynamic_cast<AMFDeviceOCLImpl*>(m_pDeviceOCL.GetPtr());

    return deviceImpl->GetComputeDevice()->GetNativeDeviceID();
}

AMF_RESULT AMFContextImpl::GetOpenCLComputeFactory(AMFComputeFactory **ppFactory)
{
    AMFComputeFactoryOCL *computeFactoryOCL = new AMFComputeFactoryOCL(this);
    computeFactoryOCL->Init();
    *ppFactory = computeFactoryOCL;
    (*ppFactory)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFContextImpl::LockOpenCL()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::UnlockOpenCL()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMF_STD_CALL AMFContextImpl::InitMetal()
{
#if defined(__APPLE__) && defined(ENABLE_METAL)

    //m_pDeviceMetal = new AMFDeviceMetalImpl(this, nullptr, this->);

    AMF_RESULT res(AMF_OK);

    amf::AMFComputeFactoryPtr oclComputeFactory;
    res = GetMetalComputeFactory(&oclComputeFactory);

    if(res == AMF_OK && oclComputeFactory->GetDeviceCount() > 0)
    {
        AMFComputeDevice* pComputeDevice = nullptr;
        res = oclComputeFactory->GetDeviceAt(0, &pComputeDevice);

        if(res == AMF_OK)
        {
            AMFComputeDeviceMetalImpl* deviceImpl = dynamic_cast<AMFComputeDeviceMetalImpl*>(pComputeDevice);
            AMFDeviceImpl* device = deviceImpl->GetDevice();

            if(device)
            {
                device->Acquire();
                m_pDeviceMetal = device;

                return AMF_OK;
            }
        }
    }

    return AMF_FAIL;

#endif

    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFContextImpl::GetMetalComputeFactory(AMFComputeFactory **ppFactory)
{
#if defined(__APPLE__) && defined(ENABLE_METAL)
    AMFComputeFactoryMetal *computeFactoryMetal = new AMFComputeFactoryMetal(this);
    computeFactoryMetal->Init();
    *ppFactory = computeFactoryMetal;
    (*ppFactory)->Acquire();
    return AMF_OK;
#endif
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFContextImpl::InitMetalEx(AMFComputeDevice *pDevice)
{
#if defined(__APPLE__) && defined(ENABLE_METAL)
    AMFComputeDeviceMetalImpl* deviceImpl = dynamic_cast<AMFComputeDeviceMetalImpl*>(pDevice);

    AMFDeviceImpl* device = deviceImpl->GetDevice();
    if (device)
    {
        device->Acquire();
        m_pDeviceMetal = device;
        return AMF_OK;
    }

    return AMF_INVALID_ARG;
#endif
    return AMF_NOT_SUPPORTED;
}

AMF_RESULT AMFContextImpl::InitOpenGL(amf_handle hOpenGLContext, amf_handle hWindow, amf_handle hDC)
{
    return AMF_NOT_IMPLEMENTED;
}

amf_handle AMFContextImpl::GetOpenGLContext()
{
    return NULL;
}

amf_handle AMFContextImpl::GetOpenGLDrawable()
{
    return NULL;
}

AMF_RESULT AMFContextImpl::LockOpenGL()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::UnlockOpenGL()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::InitXV(void *pXVDevice)
{
    return AMF_NOT_IMPLEMENTED;
}

void *AMFContextImpl::GetXVDevice()
{
    return NULL;
}

AMF_RESULT AMFContextImpl::LockXV()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::UnlockXV()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::InitGralloc(void *pGrallocDevice)
{
    return AMF_NOT_IMPLEMENTED;
}

void *AMFContextImpl::GetGrallocDevice()
{
    return NULL;
}

AMF_RESULT AMFContextImpl::LockGralloc()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::UnlockGralloc()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::AllocBuffer(AMF_MEMORY_TYPE type, amf_size size, AMFBuffer **ppBuffer)
{
    AMFBufferImpl * impl = new AMFBufferImpl(this);
    AMF_RESULT res = impl->Allocate(type, size);
    *ppBuffer = impl;
    (*ppBuffer)->Acquire();
    return res;
}

AMF_RESULT AMFContextImpl::AllocSurface(AMF_MEMORY_TYPE type, AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, AMFSurface **ppSurface)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::AllocAudioBuffer(AMF_MEMORY_TYPE type, AMF_AUDIO_FORMAT format, amf_int32 samples, amf_int32 sampleRate, amf_int32 channels, AMFAudioBuffer **ppAudioBuffer)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateBufferFromHostNative(void *pHostBuffer, amf_size size, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver)
{
	AMF_RETURN_IF_FAILED(AllocBuffer(AMF_MEMORY_HOST, size, ppBuffer));
	AMF_RETURN_IF_FAILED(GetDeviceHost()->CopyBufferFromHost((*ppBuffer)->GetNative(), 0, pHostBuffer, size, true));
    return AMF_OK;
}

AMF_RESULT AMFContextImpl::CreateSurfaceFromHostNative(AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, amf_int32 hPitch, amf_int32 vPitch, void *pData, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateSurfaceFromDX9Native(void *pDX9Surface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateSurfaceFromDX11Native(void *pDX11Surface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateSurfaceFromOpenGLNative(AMF_SURFACE_FORMAT format, amf_handle hGLTextureID, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateSurfaceFromGrallocNative(amf_handle hGrallocSurface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateSurfaceFromOpenCLNative(AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, void **pClPlanes, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateBufferFromOpenCLNative(void *pCLBuffer, amf_size size, AMFBuffer **ppBuffer)
{
	AMFBufferImpl * impl = new AMFBufferImpl(this);
	AMF_RESULT res = impl->Attach(AMF_MEMORY_OPENCL, pCLBuffer, size);
	*ppBuffer = impl;
	(*ppBuffer)->Acquire();
	return res;
}

AMF_RESULT AMFContextImpl::GetCompute(AMF_MEMORY_TYPE eMemType, AMFCompute **ppCompute)
{
#if defined(__APPLE__) && defined(ENABLE_METAL)
    if(AMF_MEMORY_METAL == eMemType)
    {
        AMFDeviceMetalImpl* deviceImpl = dynamic_cast<AMFDeviceMetalImpl*>(m_pDeviceMetal.GetPtr());

        return deviceImpl->GetComputeDevice()->CreateCompute(nullptr, ppCompute);
    }
    else
#endif

    if(AMF_MEMORY_OPENCL == eMemType)
    {
        AMFDeviceOCLImpl* deviceImpl = dynamic_cast<AMFDeviceOCLImpl*>(m_pDeviceOCL.GetPtr());

        return deviceImpl->GetComputeDevice()->CreateCompute(nullptr, ppCompute);
    }

    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateBufferFromDX11Native(void *pHostBuffer, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::AllocBufferEx(AMF_MEMORY_TYPE type, amf_size size, AMF_BUFFER_USAGE usage, AMF_MEMORY_CPU_ACCESS access, AMFBuffer **ppBuffer)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::AllocSurfaceEx(AMF_MEMORY_TYPE type, AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, AMF_SURFACE_USAGE usage, AMF_MEMORY_CPU_ACCESS access, AMFSurface **ppSurface)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::InitVulkan(void *pVulkanDevice)
{
    return AMF_NOT_IMPLEMENTED;
}

void *AMFContextImpl::GetVulkanDevice()
{
    return NULL;
}

AMF_RESULT AMFContextImpl::LockVulkan()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::UnlockVulkan()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateSurfaceFromVulkanNative(void *pVulkanImage, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::CreateBufferFromVulkanNative(void *pVulkanBuffer, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFContextImpl::GetVulkanDeviceExtensions(amf_size *pCount, const char **ppExtensions)
{
    return AMF_NOT_IMPLEMENTED;
}

AMFDevice *AMFContextImpl::GetDevice(AMF_MEMORY_TYPE type)
{
    if (type == AMF_MEMORY_HOST)
        return GetDeviceHost();
    if (type == AMF_MEMORY_OPENCL)
        return m_pDeviceOCL;
    if (type == AMF_MEMORY_METAL)
        return m_pDeviceMetal;
    return nullptr;
}

AMFDevice* AMF_STD_CALL AMFContextImpl::GetDeviceHost()
{
    if(m_pDeviceHost == NULL)
    {
        m_pDeviceHost = new AMFDeviceHostImpl(this);
    }
    return m_pDeviceHost;
}
