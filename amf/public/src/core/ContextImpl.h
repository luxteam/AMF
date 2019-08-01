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

    // AMFContext interface
public:
    virtual AMF_RESULT Terminate() override;
    virtual AMF_RESULT InitDX9(void *pDX9Device) override;
    virtual void *GetDX9Device(AMF_DX_VERSION dxVersionRequired) override;
    virtual AMF_RESULT LockDX9() override;
    virtual AMF_RESULT UnlockDX9() override;

    virtual AMF_RESULT InitDX11(void *pDX11Device, AMF_DX_VERSION dxVersionRequired) override;
    virtual void *GetDX11Device(AMF_DX_VERSION dxVersionRequired) override;
    virtual AMF_RESULT LockDX11() override;
    virtual AMF_RESULT UnlockDX11() override;

    virtual AMF_RESULT InitOpenCL(void *pCommandQueue) override;
    virtual void *GetOpenCLContext() override;
    virtual void *GetOpenCLCommandQueue() override;
    virtual void *GetOpenCLDeviceID() override;
    virtual AMF_RESULT GetOpenCLComputeFactory(AMFComputeFactory **ppFactory) override;
    virtual AMF_RESULT InitOpenCLEx(AMFComputeDevice *pDevice) override;
    virtual AMF_RESULT LockOpenCL() override;
    virtual AMF_RESULT UnlockOpenCL() override;

    virtual AMF_RESULT InitOpenGL(amf_handle hOpenGLContext, amf_handle hWindow, amf_handle hDC) override;
    virtual amf_handle GetOpenGLContext() override;
    virtual amf_handle GetOpenGLDrawable() override;
    virtual AMF_RESULT LockOpenGL() override;
    virtual AMF_RESULT UnlockOpenGL() override;

    virtual AMF_RESULT InitXV(void *pXVDevice) override;
    virtual void *GetXVDevice() override;
    virtual AMF_RESULT LockXV() override;
    virtual AMF_RESULT UnlockXV() override;

    virtual AMF_RESULT InitGralloc(void *pGrallocDevice) override;
    virtual void *GetGrallocDevice() override;
    virtual AMF_RESULT LockGralloc() override;
    virtual AMF_RESULT UnlockGralloc() override;


    virtual AMF_RESULT AllocBuffer(AMF_MEMORY_TYPE type, amf_size size, AMFBuffer **ppBuffer) override;
    virtual AMF_RESULT AllocSurface(AMF_MEMORY_TYPE type, AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, AMFSurface **ppSurface) override;
    virtual AMF_RESULT AllocAudioBuffer(AMF_MEMORY_TYPE type, AMF_AUDIO_FORMAT format, amf_int32 samples, amf_int32 sampleRate, amf_int32 channels, AMFAudioBuffer **ppAudioBuffer) override;

    virtual AMF_RESULT CreateBufferFromHostNative(void *pHostBuffer, amf_size size, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver) override;
    virtual AMF_RESULT CreateSurfaceFromHostNative(AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, amf_int32 hPitch, amf_int32 vPitch, void *pData, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;

    virtual AMF_RESULT CreateSurfaceFromDX9Native(void *pDX9Surface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT CreateSurfaceFromDX11Native(void *pDX11Surface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;

    virtual AMF_RESULT CreateSurfaceFromOpenGLNative(AMF_SURFACE_FORMAT format, amf_handle hGLTextureID, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT CreateSurfaceFromGrallocNative(amf_handle hGrallocSurface, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;

    virtual AMF_RESULT CreateSurfaceFromOpenCLNative(AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, void **pClPlanes, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT CreateBufferFromOpenCLNative(void *pCLBuffer, amf_size size, AMFBuffer **ppBuffer) override;
    virtual AMF_RESULT GetCompute(AMF_MEMORY_TYPE eMemType, AMFCompute **ppCompute) override;

    // AMFContext1 interface
public:
    virtual AMF_RESULT CreateBufferFromDX11Native(void *pHostBuffer, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver) override;
    virtual AMF_RESULT AllocBufferEx(AMF_MEMORY_TYPE type, amf_size size, AMF_BUFFER_USAGE usage, AMF_MEMORY_CPU_ACCESS access, AMFBuffer **ppBuffer) override;
    virtual AMF_RESULT AllocSurfaceEx(AMF_MEMORY_TYPE type, AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height, AMF_SURFACE_USAGE usage, AMF_MEMORY_CPU_ACCESS access, AMFSurface **ppSurface) override;

    virtual AMF_RESULT InitVulkan(void *pVulkanDevice) override;
    virtual void *GetVulkanDevice() override;
    virtual AMF_RESULT LockVulkan() override;
    virtual AMF_RESULT UnlockVulkan() override;
    virtual AMF_RESULT CreateSurfaceFromVulkanNative(void *pVulkanImage, AMFSurface **ppSurface, AMFSurfaceObserver *pObserver) override;
    virtual AMF_RESULT CreateBufferFromVulkanNative(void *pVulkanBuffer, AMFBuffer **ppBuffer, AMFBufferObserver *pObserver) override;
    virtual AMF_RESULT GetVulkanDeviceExtensions(amf_size *pCount, const char **ppExtensions) override;

public:
    AMFDevice* AMF_STD_CALL GetDevice(AMF_MEMORY_TYPE type);
    AMFDevice* AMF_STD_CALL GetDeviceHost();
private:
    AMFDevicePtr m_pDeviceHost;
};

#endif // AMFCONTEXTIMPL_H
