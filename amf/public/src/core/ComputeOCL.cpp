#include "ComputeOCL.h"

AMFComputeOCL::AMFComputeOCL(cl_device_id deviceID, cl_context context, cl_command_queue command_queue)
    :m_deviceID(deviceID), m_context(context), m_command_queue(command_queue)
{

}

AMF_MEMORY_TYPE AMFComputeOCL::GetMemoryType()
{
    return AMF_MEMORY_OPENCL;
}

void *AMFComputeOCL::GetNativeContext()
{
    return m_context;
}

void *AMFComputeOCL::GetNativeDeviceID()
{
    return m_deviceID;
}

void *AMFComputeOCL::GetNativeCommandQueue()
{
    return m_command_queue;
}

AMF_RESULT AMFComputeOCL::GetKernel(AMF_KERNEL_ID kernelID, AMFComputeKernel **kernel)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::PutSyncPoint(AMFComputeSyncPoint **ppSyncPoint)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::FinishQueue()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::FlushQueue()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::FillPlane(AMFPlane *pPlane, const amf_size origin[], const amf_size region[], const void *pColor)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::FillBuffer(AMFBuffer *pBuffer, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::ConvertPlaneToBuffer(AMFPlane *pSrcPlane, AMFBuffer **ppDstBuffer)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyBuffer(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffset)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyPlane(AMFPlane *pSrcPlane, const amf_size srcOrigin[], const amf_size region[], AMFPlane *pDstPlane, const amf_size dstOrigin[])
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyBufferToHost(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, void *pDest, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyBufferFromHost(const void *pSource, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffsetInBytes, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyPlaneToHost(AMFPlane *pSrcPlane, const amf_size origin[], const amf_size region[], void *pDest, amf_size dstPitch, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyPlaneFromHost(void *pSource, const amf_size origin[], const amf_size region[], amf_size srcPitch, AMFPlane *pDstPlane, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::ConvertPlaneToPlane(AMFPlane *pSrcPlane, AMFPlane **ppDstPlane, AMF_CHANNEL_ORDER order, AMF_CHANNEL_TYPE type)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeFactoryOCL::Init()
{
    cl_platform_id platformID;

    cl_uint numPlatforms = 0;
    cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if(0 < numPlatforms)
    {
        amf_vector<cl_platform_id> platforms;
        platforms.resize(numPlatforms);
        status = clGetPlatformIDs(numPlatforms, &platforms[0], NULL);
        AMF_RETURN_IF_CL_FAILED(status, L"clGetPlatformIDs() failed");

        for(unsigned i = 0; i < numPlatforms; ++i)
        {
            char pbuf[1000];
            status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pbuf), pbuf, NULL);
            AMF_RETURN_IF_CL_FAILED(status, L"clGetPlatformInfo() failed");
            if(!strcmp(pbuf, "Advanced Micro Devices, Inc."))
            {
                platformID = platforms[i];
                break;
            }
        }
    }
    amf_vector<cl_device_id>   deviceIDs;

    if(platformID)
    {
        cl_uint numDevices;
        std::vector<cl_context_properties> cps;

        cps.push_back(CL_CONTEXT_INTEROP_USER_SYNC);
        cps.push_back(CL_TRUE);

        cps.push_back(CL_CONTEXT_PLATFORM);
        cps.push_back((cl_context_properties)platformID);
        cps.push_back(0);

        status = clGetDeviceIDs((cl_platform_id)platformID, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
        if(numDevices)
        {
            deviceIDs.resize(numDevices);
            status = clGetDeviceIDs((cl_platform_id)platformID, CL_DEVICE_TYPE_ALL, numDevices, (cl_device_id*)&deviceIDs[0], nullptr);
        }
        for (int i = 0; i < numDevices; ++i)
        {
            cl_context context = clCreateContext(&cps[0], 1, &deviceIDs[i], NULL, NULL, &status);
            m_devices.push_back(new AMFComputeDeviceOCL(platformID, deviceIDs[i], context));
        }
    }
}

amf_int32 AMFComputeFactoryOCL::GetDeviceCount()
{
    return m_devices.size();
}

AMF_RESULT AMFComputeFactoryOCL::GetDeviceAt(amf_int32 index, AMFComputeDevice **ppDevice)
{
    //TODO: check out of range
    *ppDevice = m_devices.at(index);
    (*ppDevice)->Acquire();
    return AMF_OK;
}

//****************** AMFComputeDeviceOCL ******************
AMFComputeDeviceOCL::AMFComputeDeviceOCL(cl_platform_id platformID, cl_device_id deviceID, cl_context context)
    :m_platformID(platformID), m_deviceID(deviceID), m_context(context)
{

}

void *AMFComputeDeviceOCL::GetNativePlatform()
{
    return m_platformID;
}

void *AMFComputeDeviceOCL::GetNativeDeviceID()
{
    return m_deviceID;
}

void *AMFComputeDeviceOCL::GetNativeContext()
{
    return m_context;
}

AMF_RESULT AMFComputeDeviceOCL::CreateCompute(void *reserved, AMFCompute **ppCompute)
{
    cl_int status = 0;
    cl_command_queue commandQueue = clCreateCommandQueue(m_context, m_deviceID, (cl_command_queue_properties)NULL, &status);

    AMFComputeOCL *computeOCL = new AMFComputeOCL(m_deviceID, m_context, commandQueue);
    *ppCompute = computeOCL;
    (*ppCompute)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFComputeDeviceOCL::CreateComputeEx(void *pCommandQueue, AMFCompute **ppCompute)
{
    AMFComputeOCL *computeOCL = new AMFComputeOCL(m_deviceID, m_context, static_cast<cl_command_queue>(pCommandQueue));
    *ppCompute = computeOCL;
    (*ppCompute)->Acquire();
    return AMF_OK;
}

//****************** end AMFComputeDeviceOCL ******************


