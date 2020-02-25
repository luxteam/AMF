#include "DeviceOCLImpl.h"
#include <CL/cl.h>
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

AMFDeviceOCLImpl::AMFDeviceOCLImpl(cl_platform_id platformID, cl_device_id deviceID, AMFContextImpl *pContext, cl_context context, cl_command_queue command_queue)
    : AMFDeviceImpl(AMF_MEMORY_OPENCL, 0, pContext),
      m_platformID(platformID), m_deviceID(deviceID), m_context(context), m_command_queue(command_queue)
{
    {
        char name[256] = {0};
        clGetDeviceInfo(deviceID, CL_DEVICE_NAME, sizeof(name), name, nullptr);

        SetProperty(AMF_DEVICE_NAME, AMFVariant(name));

        fprintf(stdout, "device name: %s\n", name);
    }

    //todo: support this properties too
    //(only if device is AMD based)
    /*{
        cl_device_topology_amd pciBusInfo = {0};
        cl_int status = clGetDeviceInfo(deviceID, CL_DEVICE_TOPOLOGY_AMD, sizeof(cl_device_topology_amd), &pciBusInfo, nullptr);

        if(AMFResultIsOK(status))
        {
            fprintf(stdout, "PCI bus: %d device: %d function: %d\n", pciBusInfo.pcie.bus, pciBusInfo.pcie.device, pciBusInfo.pcie.function);
        }
    }

    {
        cl_uint max_CUs = 0;
        cl_int status = clGetDeviceInfo(deviceID, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &max_CUs, nullptr);

        if(AMFResultIsOK(status))
        {
            fprintf(stdout, "max compute units: %d\n", max_CUs);
        }
    }*/

    if (!command_queue)
    {
        cl_int status = 0;
        cl_command_queue commandQueue = nullptr;

#if CL_TARGET_OPENCL_VERSION >= 200
        commandQueue = clCreateCommandQueueWithProperties(m_context, m_deviceID, (cl_command_queue_properties)NULL, &status);
#else
        commandQueue = clCreateCommandQueue(m_context, m_deviceID, NULL, &status);
#endif

        if (!commandQueue)
        {
            printf("Error: Failed to create a commands Queue!\n");
        }
        else
            m_command_queue = commandQueue;
    }
}

AMFDeviceOCLImpl::~AMFDeviceOCLImpl()
{
	clReleaseCommandQueue(m_command_queue);
	clReleaseDevice(m_deviceID);
	clReleaseContext(m_context);
}

AMF_RESULT AMFDeviceOCLImpl::AllocateBuffer(amf_size size, void **ppHandle)
{
    return AllocateBufferEx(size, ppHandle, AMF_ARGUMENT_ACCESS_READWRITE);
}

AMF_RESULT AMFDeviceOCLImpl::AttachBuffer(amf_size size, void *pHandle)
{
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::ReleaseBuffer(void *pHandle, bool attached)
{
    if (!attached)
    {
        int err = clReleaseMemObject((cl_mem)pHandle);
        if (err != CL_SUCCESS)
        {
            printf("Error: clReleaseMemObject failed! Invalid object.");
            return AMF_FAIL;
        }
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::CreateSubBuffer(AMFBuffer * pHandle, void ** subBuffer, amf_size offset, amf_size size)
{
	AMF_RETURN_IF_FALSE(subBuffer != 0, AMF_INVALID_ARG, L"AllocateBufferEx() - ppHandle cannot be 0");

	//TODO: check type
	cl_buffer_region region;
	region.origin = offset;
	region.size = size;
	int err = 0;
	*subBuffer = clCreateSubBuffer((cl_mem)pHandle->GetNative(),
		CL_MEM_READ_WRITE,
		CL_BUFFER_CREATE_TYPE_REGION, &region, &err);
	
	if (err != CL_SUCCESS)
	{
		printf("Error: clCreateSubBuffer failed!");
		return AMF_FAIL;
	}

	return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size)
{
    int err = clEnqueueCopyBuffer(m_command_queue, (cl_mem)pSourceHandle, (cl_mem)pDestHandle, srcOffset, dstOffset, size, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to clEnqueueCopyBuffer! Code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking)
{
    int err = clEnqueueReadBuffer(m_command_queue, (cl_mem)pSourceHandle, (blocking)?CL_TRUE: CL_FALSE, srcOffset, size , (float*)pDest, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to clEnqueueReadBuffer! Code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking)
{
    int err = clEnqueueWriteBuffer(m_command_queue, (cl_mem)pDestHandle, (blocking)?CL_TRUE: CL_FALSE, dstOffset, size, (const float*)pSource, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to clEnqueueWriteBuffer! Code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    int err = clEnqueueFillBuffer (m_command_queue, (cl_mem)pDestHandle, pSourcePattern, patternSize, dstOffset, dstSize, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to clEnqueueFillBuffer! Code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::AllocateBufferEx(amf_size size, void **ppHandle, AMF_ARGUMENT_ACCESS_TYPE format)
{
    AMF_RETURN_IF_FALSE(size != 0, AMF_INVALID_ARG, L"AllocateBufferEx() - size cannot be 0");
    AMF_RETURN_IF_FALSE(ppHandle != 0, AMF_INVALID_ARG, L"AllocateBufferEx() - ppHandle cannot be 0");
    *ppHandle = clCreateBuffer(m_context, amf_to_cl_format(format), size, NULL, NULL);
    AMF_RETURN_IF_FALSE(*ppHandle != nullptr, AMF_OUT_OF_MEMORY, L"Error: Failed to allocate device memory!\n");
    return AMF_OK;
}

AMF_MEMORY_TYPE AMFDeviceOCLImpl::GetMemoryType()
{
    return GetType();
}

void *AMFDeviceOCLImpl::GetNativeContext()
{
    return m_context;
}

void *AMFDeviceOCLImpl::GetNativeDeviceID()
{
    return m_deviceID;
}

void *AMFDeviceOCLImpl::GetNativeCommandQueue()
{
    return m_command_queue;
}

AMF_RESULT AMFDeviceOCLImpl::GetKernel(AMF_KERNEL_ID kernelID, AMFComputeKernel **kernel)
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

    kernel_CL = clCreateKernel(program, kernelData->kernelName, &err);
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

AMF_RESULT AMFDeviceOCLImpl::PutSyncPoint(AMFComputeSyncPoint **ppSyncPoint)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceOCLImpl::FinishQueue()
{
    int err = 0;
    err = clFinish(m_command_queue);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to FinishQueue! Error code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::FlushQueue()
{
    int err = 0;
    err = clFlush(m_command_queue);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to Flush Queue! Error code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::FillPlane(AMFPlane *pPlane, const amf_size origin[], const amf_size region[], const void *pColor)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceOCLImpl::FillBuffer(AMFBuffer *pBuffer, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    //TODO: check
    return FillBuffer(pBuffer->GetNative(), dstOffset, dstSize, pSourcePattern, patternSize);
}

AMF_RESULT AMFDeviceOCLImpl::ConvertPlaneToBuffer(AMFPlane *pSrcPlane, AMFBuffer **ppDstBuffer)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceOCLImpl::CopyBuffer(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffset)
{
    //TODO: memory type
    return CopyBuffer(pDstBuffer->GetNative(), dstOffset, pSrcBuffer->GetNative(), srcOffset, size);
}

AMF_RESULT AMFDeviceOCLImpl::CopyPlane(AMFPlane *pSrcPlane, const amf_size srcOrigin[], const amf_size region[], AMFPlane *pDstPlane, const amf_size dstOrigin[])
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceOCLImpl::CopyBufferToHost(AMFBuffer *pSrcBuffer, amf_size srcOffset, amf_size size, void *pDest, amf_bool blocking)
{
    //TODO: memory type check
    return CopyBufferToHost(pDest, pSrcBuffer->GetNative(), srcOffset, size, blocking);
}

AMF_RESULT AMFDeviceOCLImpl::CopyBufferFromHost(const void *pSource, amf_size size, AMFBuffer *pDstBuffer, amf_size dstOffsetInBytes, amf_bool blocking)
{
    //TODO: memory type check
    return CopyBufferFromHost(pDstBuffer, dstOffsetInBytes, pSource, size, blocking);
}

AMF_RESULT AMFDeviceOCLImpl::CopyPlaneToHost(AMFPlane *pSrcPlane, const amf_size origin[], const amf_size region[], void *pDest, amf_size dstPitch, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceOCLImpl::CopyPlaneFromHost(void *pSource, const amf_size origin[], const amf_size region[], amf_size srcPitch, AMFPlane *pDstPlane, amf_bool blocking)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceOCLImpl::ConvertPlaneToPlane(AMFPlane *pSrcPlane, AMFPlane **ppDstPlane, AMF_CHANNEL_ORDER order, AMF_CHANNEL_TYPE type)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFDeviceOCLImpl::CreateCompute(void *reserved, AMFCompute **ppCompute)
{
    cl_int status = 0;
    cl_command_queue commandQueue = nullptr;

#if CL_TARGET_OPENCL_VERSION >= 200
        commandQueue = clCreateCommandQueueWithProperties(m_context, m_deviceID, (cl_command_queue_properties)NULL, &status);
#else
        commandQueue = clCreateCommandQueue(m_context, m_deviceID, NULL, &status);
#endif

    if (!commandQueue)
    {
        printf("Error: Failed to create a commands Queue!\n");
        return AMF_FAIL;
    }
    AMFDeviceOCLImpl *computeOCL = new AMFDeviceOCLImpl(m_platformID, m_deviceID, m_pContext, m_context, commandQueue);
    *ppCompute = computeOCL;
    (*ppCompute)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::CreateComputeEx(void *pCommandQueue, AMFCompute **ppCompute)
{
    AMFDeviceOCLImpl *computeOCL = new AMFDeviceOCLImpl(m_platformID, m_deviceID, m_pContext, m_context, (cl_command_queue)pCommandQueue);
    *ppCompute = computeOCL;
    (*ppCompute)->Acquire();
    return AMF_OK;
}

void *AMFDeviceOCLImpl::GetNativePlatform()
{
    return NULL;
}
