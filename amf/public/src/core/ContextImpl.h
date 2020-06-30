#ifndef AMFCONTEXTIMPL_H
#define AMFCONTEXTIMPL_H

#include "../../include/core/Context.h"
#include "../../common/InterfaceImpl.h"
#include "../../common/PropertyStorageImpl.h"
#include "Device.h"

using namespace amf;

class AMFContextImpl : public AMFInterfaceImpl<AMFPropertyStorageImpl<AMFContext1> >
{
public:
    AMFContextImpl();
    virtual ~AMFContextImpl();
    // AMFContext interface
public:
    virtual AMF_RESULT AMF_STD_CALL Terminate() override;
    virtual AMF_RESULT AMF_STD_CALL InitDX9(void *pDX9Device) override;
    virtual void * AMF_STD_CALL GetDX9Device(AMF_DX_VERSION dxVersionRequired) override;
    virtual AMF_RESULT AMF_STD_CALL LockDX9() override;
    virtual AMF_RESULT AMF_STD_CALL UnlockDX9() override;

    virtual AMF_RESULT AMF_STD_CALL InitDX11(void *pDX11Device, AMF_DX_VERSION dxVersionRequired) override;
    virtual void * AMF_STD_CALL GetDX11Device(AMF_DX_VERSION dxVersionRequired) override;
    virtual AMF_RESULT AMF_STD_CALL LockDX11() override;
    virtual AMF_RESULT AMF_STD_CALL UnlockDX11() override;

    virtual AMF_RESULT AMF_STD_CALL InitOpenCL(void *pCommandQueue) override;
    virtual void * AMF_STD_CALL GetOpenCLContext() override;
    virtual void * AMF_STD_CALL GetOpenCLCommandQueue() override;
    virtual void * AMF_STD_CALL GetOpenCLDeviceID() override;
    virtual AMF_RESULT AMF_STD_CALL GetOpenCLComputeFactory(AMFComputeFactory **ppFactory) override;
    virtual AMF_RESULT AMF_STD_CALL InitOpenCLEx(AMFComputeDevice *pDevice) override;
    virtual AMF_RESULT AMF_STD_CALL LockOpenCL() override;
    virtual AMF_RESULT AMF_STD_CALL UnlockOpenCL() override;

    virtual AMF_RESULT AMF_STD_CALL InitOpenGL(amf_handle hOpenGLContext, amf_handle hWindow, amf_handle hDC) override;
    virtual amf_handle AMF_STD_CALL GetOpenGLContext() override;
    virtual amf_handle AMF_STD_CALL GetOpenGLDrawable() override;
    virtual AMF_RESULT AMF_STD_CALL LockOpenGL() override;
    virtual AMF_RESULT AMF_STD_CALL UnlockOpenGL() override;

    virtual AMF_RESULT AMF_STD_CALL InitXV(void *pXVDevice) override;
    virtual void * AMF_STD_CALL GetXVDevice() override;
    virtual AMF_RESULT AMF_STD_CALL LockXV() override;
    virtual AMF_RESULT AMF_STD_CALL UnlockXV() override;

    virtual AMF_RESULT AMF_STD_CALL InitGralloc(void *pGrallocDevice) override;
    virtual void * AMF_STD_CALL GetGrallocDevice() override;
    virtual AMF_RESULT AMF_STD_CALL LockGralloc() override;
    virtual AMF_RESULT AMF_STD_CALL UnlockGralloc() override;


    virtual AMF_RESULT AMF_STD_CALL AllocBuffer(AMF_MEMORY_TYPE type, amf_size size, AMFBuffer **ppBuffer) override;
    virtual AMF_RESULT AMF_STD_CALL AllocSurface(AMF_MEMORY_TYPE type, AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, AMFSurface **ppSurface) override;
    virtual AMF_RESULT AMF_STD_CALL AllocAudioBuffer(AMF_MEMORY_TYPE type, AMF_AUDIO_FORMAT format, amf_int32 samples, amf_int32 sampleRate, amf_int32 channels, AMFAudioBuffer **ppAudioBuffer) override;

    virtual AMF_RESULT AMF_STD_CALL CreateBufferFromHostNative(void *pHostBuffer, amf_size size, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver) override;
    virtual AMF_RESULT AMF_STD_CALL CreateSurfaceFromHostNative(AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, amf_int32 hPitch, amf_int32 vPitch, void *pData, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;

    virtual AMF_RESULT AMF_STD_CALL CreateSurfaceFromDX9Native(void *pDX9Surface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT AMF_STD_CALL CreateSurfaceFromDX11Native(void *pDX11Surface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;

    virtual AMF_RESULT AMF_STD_CALL CreateSurfaceFromOpenGLNative(AMF_SURFACE_FORMAT format, amf_handle hGLTextureID, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT AMF_STD_CALL CreateSurfaceFromGrallocNative(amf_handle hGrallocSurface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;

    virtual AMF_RESULT AMF_STD_CALL CreateSurfaceFromOpenCLNative(AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, void **pClPlanes, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT AMF_STD_CALL CreateBufferFromOpenCLNative(void *pCLBuffer, amf_size size, AMFBuffer **ppBuffer) override;
    virtual AMF_RESULT AMF_STD_CALL GetCompute(AMF_MEMORY_TYPE eMemType, AMFCompute **ppCompute) override;

    // AMFContext1 interface
public:
    virtual AMF_RESULT AMF_STD_CALL CreateBufferFromDX11Native(void *pHostBuffer, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver) override;
    virtual AMF_RESULT AMF_STD_CALL AllocBufferEx(AMF_MEMORY_TYPE type, amf_size size, AMF_BUFFER_USAGE usage, AMF_MEMORY_CPU_ACCESS access, AMFBuffer **ppBuffer) override;
    virtual AMF_RESULT AMF_STD_CALL AllocSurfaceEx(AMF_MEMORY_TYPE type, AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, AMF_SURFACE_USAGE usage, AMF_MEMORY_CPU_ACCESS access, AMFSurface **ppSurface) override;

    virtual AMF_RESULT AMF_STD_CALL InitVulkan(void *pVulkanDevice) override;
    virtual void * AMF_STD_CALL GetVulkanDevice() override;
    virtual AMF_RESULT AMF_STD_CALL LockVulkan() override;
    virtual AMF_RESULT AMF_STD_CALL UnlockVulkan() override;
    virtual AMF_RESULT AMF_STD_CALL CreateSurfaceFromVulkanNative(void *pVulkanImage, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT AMF_STD_CALL CreateBufferFromVulkanNative(void *pVulkanBuffer, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver) override;
    virtual AMF_RESULT AMF_STD_CALL GetVulkanDeviceExtensions(amf_size *pCount, const char **ppExtensions) override;

public:
    AMFDevice* AMF_STD_CALL GetDevice(AMF_MEMORY_TYPE type);
    AMFDevice* AMF_STD_CALL GetDeviceHost();
private:
    AMFDevicePtr m_pDeviceHost;
    AMFDevicePtr m_pDeviceOCL;
};

#endif // AMFCONTEXTIMPL_H
