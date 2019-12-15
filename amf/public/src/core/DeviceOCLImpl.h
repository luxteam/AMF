#ifndef DEVICEOCLIMPL_H
#define DEVICEOCLIMPL_H
#include "DeviceImpl.h"
#include "ContextImpl.h"
#include <CL/cl.h>

using namespace amf;

class AMFDeviceOCLImpl : public AMFDeviceImpl,
        public AMFInterfaceImpl<AMFCompute>,
        public AMFInterfaceImpl<AMFPropertyStorageImpl<AMFComputeDevice>>

{
public:
    AMFDeviceOCLImpl(cl_platform_id platformID, cl_device_id deviceID, AMFContextImpl *pContext, cl_context context, cl_command_queue command_queue = NULL);

    // AMFDevice interface
public:
    virtual AMF_RESULT AllocateBuffer(amf_size size, void **ppHandle) override;
    virtual AMF_RESULT AttachBuffer(amf_size size, void *pHandle) override;
    virtual AMF_RESULT ReleaseBuffer(void *pHandle, bool attached) override;

    virtual AMF_RESULT CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size) override;
    virtual AMF_RESULT CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking) override;
    virtual AMF_RESULT CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking) override;
    virtual AMF_RESULT FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize) override;

    //
    AMF_RESULT AllocateBufferEx(amf_size size, void **ppHandle, enum AMF_ARGUMENT_ACCESS_TYPE format);

    // AMFCompute interface
public:
    virtual AMF_MEMORY_TYPE GetMemoryType() override;
    virtual void *GetNativeContext() override;
    virtual void *GetNativeDeviceID() override;
    virtual void *GetNativeCommandQueue() override;
    virtual AMF_RESULT GetKernel(AMF_KERNEL_ID kernelID, AMFComputeKernel **kernel) override;
    virtual AMF_RESULT PutSyncPoint(AMFComputeSyncPoint **ppSyncPoint) override;
    virtual AMF_RESULT FinishQueue() override;
    virtual AMF_RESULT FlushQueue() override;
    virtual AMF_RESULT FillPlane(AMFPlane *pPlane, const amf_size origin[], const amf_size region[], const void *pColor) override;
    virtual AMF_RESULT FillBuffer(AMFBuffer *pBuffer, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize) override;
    virtual AMF_RESULT ConvertPlaneToBuffer(AMFPlane *pSrcPlane, AMFBuffer **ppDstBuffer) override;
    virtual AMF_RESULT CopyBuffer(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffset) override;
    virtual AMF_RESULT CopyPlane(AMFPlane *pSrcPlane, const amf_size srcOrigin[], const amf_size region[], AMFPlane *pDstPlane, const amf_size dstOrigin[]) override;
    virtual AMF_RESULT CopyBufferToHost(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, void *pDest, amf_bool blocking) override;
    virtual AMF_RESULT CopyBufferFromHost(const void *pSource, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffsetInBytes, amf_bool blocking) override;
    virtual AMF_RESULT CopyPlaneToHost(AMFPlane *pSrcPlane, const amf_size origin[], const amf_size region[], void *pDest, amf_size dstPitch, amf_bool blocking) override;
    virtual AMF_RESULT CopyPlaneFromHost(void *pSource, const amf_size origin[], const amf_size region[], amf_size srcPitch, AMFPlane *pDstPlane, amf_bool blocking) override;
    virtual AMF_RESULT ConvertPlaneToPlane(AMFPlane *pSrcPlane, AMFPlane **ppDstPlane, AMF_CHANNEL_ORDER order, AMF_CHANNEL_TYPE type) override;

    // AMFComputeDevice interface
public:
    virtual AMF_RESULT CreateCompute(void *reserved, AMFCompute **ppCompute) override;
    virtual AMF_RESULT CreateComputeEx(void *pCommandQueue, AMFCompute **ppCompute) override;
    virtual void *GetNativePlatform() override;
private:
    cl_platform_id m_platformID;
    cl_device_id m_deviceID;
    cl_context m_context;
    cl_command_queue m_command_queue;
};

#endif // DEVICEOCLIMPL_H
