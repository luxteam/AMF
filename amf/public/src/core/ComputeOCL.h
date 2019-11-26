#ifndef AMFCOMPUTEOCL_H
#define AMFCOMPUTEOCL_H

#include "../../include/core/Compute.h"
#include "../../include/core/ComputeFactory.h"
#include "../../common/InterfaceImpl.h"
#include "../../common/AMFSTL.h"
#include "../../common/TraceAdapter.h"
#include "../../common/PropertyStorageImpl.h"
#include <CL/cl.h>
#include "DeviceOCLImpl.h"

using namespace amf;

class AMFComputeKernelOCL : public AMFInterfaceImpl<AMFComputeKernel>
{
public:
    AMFComputeKernelOCL(cl_program program, cl_kernel kernel, cl_command_queue command_queue, AMF_KERNEL_ID kernelID, cl_device_id deviceID, cl_context context);

    virtual void*               AMF_STD_CALL GetNative();
    virtual const wchar_t*      AMF_STD_CALL GetIDName();

    virtual AMF_RESULT          AMF_STD_CALL SetArgPlaneNative(amf_size index, void* pPlane, AMF_ARGUMENT_ACCESS_TYPE eAccess);
    virtual AMF_RESULT          AMF_STD_CALL SetArgBufferNative(amf_size index, void* pBuffer, AMF_ARGUMENT_ACCESS_TYPE eAccess);

    virtual AMF_RESULT          AMF_STD_CALL SetArgPlane(amf_size index, AMFPlane* pPlane, AMF_ARGUMENT_ACCESS_TYPE eAccess);
    virtual AMF_RESULT          AMF_STD_CALL SetArgBuffer(amf_size index, AMFBuffer* pBuffer, AMF_ARGUMENT_ACCESS_TYPE eAccess);

    virtual AMF_RESULT          AMF_STD_CALL SetArgInt32(amf_size index, amf_int32 data);
    virtual AMF_RESULT          AMF_STD_CALL SetArgInt64(amf_size index, amf_int64 data);
    virtual AMF_RESULT          AMF_STD_CALL SetArgFloat(amf_size index, amf_float data);
    virtual AMF_RESULT          AMF_STD_CALL SetArgBlob(amf_size index, amf_size dataSize, const void* pData);

    virtual AMF_RESULT          AMF_STD_CALL GetCompileWorkgroupSize(amf_size workgroupSize[3]);

    virtual AMF_RESULT          AMF_STD_CALL Enqueue(amf_size dimension, amf_size globalOffset[3], amf_size globalSize[3], amf_size localSize[3]);
private:
    AMF_KERNEL_ID m_kernelID;
    cl_kernel m_kernel;
    cl_context m_context;
    cl_program m_program;
    cl_command_queue m_command_queue;
    cl_device_id m_deviceID;
    struct BufferDesc
    {
        cl_mem buffer;
        void *data;
        uint8_t access;
        int index;
        int size;
    };

    std::vector<BufferDesc> m_buffers;
};

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
    AMFDeviceOCLImpl * m_deviceImpl;
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
