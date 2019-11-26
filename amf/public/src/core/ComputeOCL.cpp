#include "ComputeOCL.h"
#include "ProgramsImpl.h"

static uint32_t amf_to_cl_format(enum AMF_ARGUMENT_ACCESS_TYPE format)
{
    if (format == AMF_ARGUMENT_ACCESS_READWRITE)
        return CL_MEM_READ_WRITE;
    if (format == AMF_ARGUMENT_ACCESS_WRITE)
        return CL_MEM_WRITE_ONLY;
    if (format == AMF_ARGUMENT_ACCESS_READ)
        return CL_MEM_READ_ONLY;

    return CL_MEM_READ_ONLY;
}

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
    AMFKernelStorage::KernelData *kernelData;
    AMFKernelStorage::Instance()->GetKernelData(&kernelData, kernelID);
    cl_program program;
    cl_kernel kernel_CL;
    int err;

    const char * source = (const char *)kernelData->data;
    program = clCreateProgramWithSource(m_context, 1, &source, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return AMF_FAIL;
    }

    // Build the program executable
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, m_deviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return AMF_FAIL;
    }

    // Create the compute kernel in the program we wish to run
    //kernel_CL = clCreateKernel(program, kernelData->kernelName, &err);
    kernel_CL = clCreateKernel(program, "square", &err);
    if (!kernel_CL || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernel!\n");
        return AMF_FAIL;
    }

    AMFComputeKernelOCL * computeKernel = new AMFComputeKernelOCL(program, kernel_CL, m_command_queue, kernelID, m_deviceID, m_context);
    *kernel = computeKernel;
    (*kernel)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFComputeOCL::PutSyncPoint(AMFComputeSyncPoint **ppSyncPoint)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::FinishQueue()
{
    clFinish(m_command_queue);
    return AMF_OK;
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
    return m_deviceImpl->FillBuffer(pBuffer, dstOffset, dstSize, pSourcePattern, patternSize);
}

AMF_RESULT AMFComputeOCL::ConvertPlaneToBuffer(AMFPlane *pSrcPlane, AMFBuffer **ppDstBuffer)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyBuffer(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffset)
{
    return m_deviceImpl->CopyBuffer(pDstBuffer, dstOffset, pSrcBuffer, srcOffset, size);
}

AMF_RESULT AMFComputeOCL::CopyPlane(AMFPlane *pSrcPlane, const amf_size srcOrigin[], const amf_size region[], AMFPlane *pDstPlane, const amf_size dstOrigin[])
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeOCL::CopyBufferToHost(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, void *pDest, amf_bool blocking)
{
    return m_deviceImpl->CopyBufferToHost(pDest, pSrcBuffer, srcOffset, size, blocking);
}

AMF_RESULT AMFComputeOCL::CopyBufferFromHost(const void *pSource, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffsetInBytes, amf_bool blocking)
{
    return m_deviceImpl->CopyBufferFromHost(pDstBuffer, dstOffsetInBytes, pSource, size, blocking);
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
    cl_command_queue commandQueue = clCreateCommandQueueWithProperties(m_context, m_deviceID, (cl_command_queue_properties)NULL, &status);
    if (!commandQueue)
    {
        printf("Error: Failed to create a commands Queue!\n");
        return AMF_FAIL;
    }
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


//****************** AMFComputeKernelOCL ******************
void *AMFComputeKernelOCL::GetNative()
{
    return m_kernel;
}

AMF_RESULT AMFComputeKernelOCL::Enqueue(amf_size dimension, amf_size globalOffset[], amf_size globalSize[], amf_size localSize[])
{
    int err = 0;
    err = clEnqueueNDRangeKernel(m_command_queue, m_kernel, dimension, &globalOffset[0], &globalSize[0], &localSize[0], 0, NULL, NULL);
    if (err)
    {
        printf("Error: Failed to execute kernel!\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFComputeKernelOCL::GetCompileWorkgroupSize(amf_size workgroupSize[])
{
    int err = 0;
    amf_size res[3];
    err = clGetKernelWorkGroupInfo(m_kernel, m_deviceID, CL_KERNEL_WORK_GROUP_SIZE, 3 * sizeof(size_t), &res, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        return AMF_FAIL;
    }
    workgroupSize[0] = res[0];
    workgroupSize[1] = res[1];
    workgroupSize[2] = res[2];

    return AMF_OK;
}

AMF_RESULT AMFComputeKernelOCL::SetArgBlob(amf_size index, amf_size dataSize, const void *pData)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeKernelOCL::SetArgFloat(amf_size index, amf_float data)
{
    int err = 0;
    err = clSetKernelArg(m_kernel, index, sizeof(amf_float), &data);
    if (err != 0)
    {
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFComputeKernelOCL::SetArgInt64(amf_size index, amf_int64 data)
{
    int err = 0;
    err = clSetKernelArg(m_kernel, index, sizeof(amf_int64), &data);
    if (err != 0)
    {
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFComputeKernelOCL::SetArgInt32(amf_size index, amf_int32 data)
{
    int err = 0;
    err = clSetKernelArg(m_kernel, index, sizeof(amf_int32), &data);
    if (err != 0)
    {
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFComputeKernelOCL::SetArgBuffer(amf_size index, AMFBuffer *pBuffer, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    int err = 0;
    if (pBuffer->GetMemoryType() != AMF_MEMORY_OPENCL)
    {
        pBuffer->Convert(AMF_MEMORY_OPENCL);
    }

    err = clSetKernelArg(m_kernel, index, sizeof(cl_mem), pBuffer->GetNative());
    if (err != 0)
    {
        printf("Error: Failed to setup arg buffer!\n index = %d", err, index);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFComputeKernelOCL::SetArgPlane(amf_size index, AMFPlane *pPlane, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFComputeKernelOCL::SetArgBufferNative(amf_size index, void *pBuffer, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    int err = 0;

    err = clSetKernelArg(m_kernel, index, sizeof(cl_mem), (cl_mem)pBuffer);
    if (err != 0)
    {
        printf("Error: Failed to setup arg buffer!\n index = %d", err, index);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFComputeKernelOCL::SetArgPlaneNative(amf_size index, void *pPlane, AMF_ARGUMENT_ACCESS_TYPE eAccess)
{
    return AMF_NOT_IMPLEMENTED;
}

const wchar_t *AMFComputeKernelOCL::GetIDName()
{
    return L"AMFComputeKernelOCL";
}

AMFComputeKernelOCL::AMFComputeKernelOCL(cl_program program, cl_kernel kernel, cl_command_queue command_queue, AMF_KERNEL_ID kernelID, cl_device_id deviceID, cl_context context)
    : m_program (program), m_kernel(kernel), m_kernelID(kernelID), m_command_queue(command_queue), m_deviceID(deviceID), m_context(context)
{


}
