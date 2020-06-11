#include "DeviceOCLImpl.h"
#include <CL/cl.h>
#include "ComputeOCL.h"
#include "ProgramsImpl.h"
#include <fstream>

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

AMFDeviceOCLImpl::AMFDeviceOCLImpl(
    cl_platform_id      platformID,
    cl_device_id        deviceID,
    AMFContextImpl      *pContext,
    cl_context          context,
    cl_command_queue    command_queue
    ):
    AMFDeviceImpl(AMF_MEMORY_OPENCL, 0, pContext),
    m_platformID(platformID),
    m_deviceID(deviceID),
    m_context(context),
    m_command_queue(command_queue)
{
    {
        char name[256] = {0};
        if(CL_SUCCESS == clGetDeviceInfo(deviceID, CL_DEVICE_NAME, sizeof(name), name, nullptr))
        {
            SetProperty(AMF_DEVICE_NAME, AMFVariant(name));
        }

    }

    {
        cl_uint align = 0;

        if(CL_SUCCESS == clGetDeviceInfo(m_deviceID, CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(align), &align, NULL))
        {
            if(align > 0)
            {
                SetProperty(AMF_DEVICE_BASEMEMORY_ALIGN, AMFVariant(align));
            }
        }
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

    //printf("\ncreate subbuffer: %llx\n", *subBuffer);

	if (err != CL_SUCCESS)
	{
		printf("Error: clCreateSubBuffer failed!");
		return AMF_FAIL;
	}

	return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::MapToHost(AMFBuffer * pHandle, void ** memory, amf_size offset, amf_size size, bool blocking)
{
	//TODO:: check type
	int err = CL_SUCCESS;
	(*memory) = clEnqueueMapBuffer(m_command_queue, (cl_mem)pHandle->GetNative(), blocking, CL_MAP_READ, offset, size, 0, NULL, NULL, &err);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to clEnqueueMapBuffer! Code = %d\n", err);
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

char * DeviceName(cl_device_id device)
{
	size_t size;
	clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &size);
	char * deviceName = (char *)malloc(size * sizeof(char));
	clGetDeviceInfo(device, CL_DEVICE_NAME, size, deviceName, 0);
	return deviceName;
}

bool SaveProgramBinary(cl_program program, cl_device_id device, const wchar_t* name, const char *kernelName)
{
	cl_uint numDevices = 0;
	cl_int errNum;

	errNum = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES,
		sizeof(cl_uint),
		&numDevices, NULL);
	if (errNum != CL_SUCCESS)
	{
		printf("Error querying for number of devices.\n");
		return false;
	}

	cl_device_id *devices = new cl_device_id[numDevices];
	errNum = clGetProgramInfo(program, CL_PROGRAM_DEVICES,
		sizeof(cl_device_id) * numDevices,
		devices, NULL);
	if (errNum != CL_SUCCESS)
	{
		printf("Error querying for devices.\n");
		delete[] devices;
		return false;
	}

	size_t *programBinarySizes = new size_t[numDevices];
	errNum = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES,
		sizeof(size_t) * numDevices,
		programBinarySizes, NULL);
	if (errNum != CL_SUCCESS)
	{
		printf("Error querying for program binary sizes.\n");
		delete[] devices;
		delete[] programBinarySizes;
		return false;
	}
	unsigned char **programBinaries =
		new unsigned char*[numDevices];
	for (cl_uint i = 0; i < numDevices; i++)
	{
		programBinaries[i] =
			new unsigned char[programBinarySizes[i]];
	}
	
	errNum = clGetProgramInfo(program, CL_PROGRAM_BINARIES,
		sizeof(unsigned char*) * numDevices,
		programBinaries, NULL);
	if (errNum != CL_SUCCESS)
	{
		printf("Error querying for program binaries\n");
		delete[] devices;
		delete[] programBinarySizes;
		for (cl_uint i = 0; i < numDevices; i++)
		{
			delete[] programBinaries[i];
		}
		delete[] programBinaries;
		return false;
	}
	char *deviceName = DeviceName(device);

	AMF_RESULT res;
	for (cl_uint i = 0; i < numDevices; i++)
	{
		if (devices[i] == device)
		{
			res = AMFKernelStorage::Instance()->SaveProgramBinary(name, kernelName, deviceName, ".cl.bin", programBinaries[i], programBinarySizes[i]);
			break;
		}
	}
	
	delete[] devices;
	delete[] programBinarySizes;
	for (cl_uint i = 0; i < numDevices; i++)
	{
		delete[] programBinaries[i];
	}
	delete[] programBinaries;
	free(deviceName);
	return res == AMF_OK;;
}

cl_program CreateProgramFromBinary(cl_context context, cl_device_id device, unsigned char *programBinary, size_t binarySize)
{
	cl_int errNum = 0;
	cl_program program;
	cl_int binaryStatus;
	program = clCreateProgramWithBinary(context,
		1,
		&device,
		&binarySize,
		(const unsigned char**)&programBinary,
		&binaryStatus,
		&errNum);
	delete[] programBinary;
	if (errNum != CL_SUCCESS)
	{
		printf("Error loading program binary.\n");
		return NULL;
	}
	if (binaryStatus != CL_SUCCESS)
	{
		printf("Invalid binary for device\n");
		return NULL;
	}
	errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (errNum != CL_SUCCESS)
	{
		// Determine the reason for the error
		char buildLog[16384];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			sizeof(buildLog), buildLog, NULL);
		printf("Error in program: \n");
		printf(buildLog);
		clReleaseProgram(program);
		return NULL;
	}
	return program;
}

AMF_RESULT AMFDeviceOCLImpl::GetKernel(AMF_KERNEL_ID kernelID, AMFComputeKernel **kernel)
{
	AMFKernelStorage::KernelData *kernelData;
	AMF_RESULT res;
	cl_kernel kernel_CL;
	int err;

	res = AMFKernelStorage::Instance()->GetKernelData(&kernelData, kernelID);
	if (res != AMF_OK) {
		return res;
	}
    cl_program program;
	char * deviceName = DeviceName(m_deviceID);
	if (kernelData->type == AMFKernelStorage::KernelData::Source)
	{
		AMFKernelStorage::KernelData *cacheKernelData;
		res = AMFKernelStorage::Instance()->GetCacheKernelData(&cacheKernelData, kernelData->kernelid_name, kernelData->kernelName ,deviceName, ".cl.bin");
		free(deviceName);
		if (res == AMF_OK)
			kernelData = cacheKernelData;//now kernelData->type == AMFKernelStorage::KernelData::Binary
	}
	if (kernelData->type == AMFKernelStorage::KernelData::Binary)
	{
		program = CreateProgramFromBinary(m_context, m_deviceID, (unsigned char*)kernelData->data, kernelData->dataSize);
		if (program)
		{
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
		else
		{
			printf("Error: Failed to create compute program from binary!\n");
			amf_int64 sourceKernelId = AMFKernelStorage::Instance()->FindSourceIndex(kernelData->kernelid_name, kernelData->options);
			if (sourceKernelId < 0)
			{
				printf("Error: Failed to find kernel source!\n");
				return AMF_FAIL;
			}
			res = AMFKernelStorage::Instance()->GetKernelData(&kernelData, sourceKernelId);
			if (res != AMF_OK) {
				printf("Error: Failed to fetch kernel source!\n");
				return res;
			}
		}
	}
    
    const char * source = (const char *)kernelData->data;
    program = clCreateProgramWithSource(m_context, 1, &source, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return AMF_FAIL;
    }

    err = clBuildProgram(program, 0, NULL, kernelData->options, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, m_deviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return AMF_FAIL;
    }
	if (AMFKernelStorage::Instance()->GetCacheFolder() != nullptr)
		SaveProgramBinary(program, m_deviceID, kernelData->kernelid_name, kernelData->kernelName);

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
    //printf("\n\nsrc: %llx dst: %llx\n\n", pSrcBuffer, pDstBuffer);

    auto source(pSrcBuffer->GetNative());
    auto dest(pDstBuffer->GetNative());

    //TODO: memory type
    return CopyBuffer(dest, dstOffset, source, srcOffset, size);
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
    cl_mem native((cl_mem)pDstBuffer->GetNative());
    //printf("\nCopyBufferFromHost: %llx\n", native);

    //TODO: memory type check
    return CopyBufferFromHost(native, dstOffsetInBytes, pSource, size, blocking);
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
