#ifndef AMFCOMPUTEOCL_H
#define AMFCOMPUTEOCL_H

#include "../../include/core/Compute.h"
#include "../../include/core/ComputeFactory.h"
#include "../../common/InterfaceImpl.h"
#include "../../common/AMFSTL.h"
#include "../../common/TraceAdapter.h"
#include "../../common/PropertyStorageImpl.h"
#include <CL/cl.h>

using namespace amf;

class AMFComputeOCL : public AMFInterfaceImpl<AMFCompute>
{
public:
    AMFComputeOCL(cl_device_id deviceID, cl_context context, cl_command_queue command_queue);

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
private:
    cl_device_id m_deviceID;
    cl_context m_context;
    cl_command_queue m_command_queue;
};

class AMFComputeDeviceOCL : public AMFInterfaceImpl<AMFPropertyStorageImpl<AMFComputeDevice>>
{
public:
    AMFComputeDeviceOCL(cl_platform_id platformID, cl_device_id deviceID, cl_context context);

    // AMFComputeDevice interface
public:
    virtual void *GetNativePlatform() override;
    virtual void *GetNativeDeviceID() override;
    virtual void *GetNativeContext() override;
    virtual AMF_RESULT CreateCompute(void *reserved, AMFCompute **ppCompute) override;
    virtual AMF_RESULT CreateComputeEx(void *pCommandQueue, AMFCompute **ppCompute) override;

private:
    cl_platform_id m_platformID;
    cl_device_id m_deviceID;
    cl_context m_context;
};

class AMFComputeFactoryOCL : public AMFInterfaceImpl<AMFComputeFactory>
{
public:
    AMF_RESULT Init();
    virtual amf_int32           AMF_STD_CALL GetDeviceCount() override;
    virtual AMF_RESULT          AMF_STD_CALL GetDeviceAt(amf_int32 index, AMFComputeDevice **ppDevice) override;
private:
    amf_vector<AMFComputeDevicePtr> m_devices;
};
#endif // AMFCOMPUTEOCL_H
