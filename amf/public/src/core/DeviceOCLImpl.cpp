#include "DeviceOCLImpl.h"
#include <CL/cl.h>

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

AMFDeviceOCLImpl::AMFDeviceOCLImpl(AMFContextImpl *pContext)
    : AMFDeviceImpl(AMF_MEMORY_OPENCL, 0, pContext)
{

}

AMF_MEMORY_TYPE AMFDeviceOCLImpl::GetType()
{
    return AMF_MEMORY_OPENCL;
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

AMF_RESULT AMFDeviceOCLImpl::CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size)
{
    int err = clEnqueueCopyBuffer((cl_command_queue)m_pContext->GetOpenCLCommandQueue(), (cl_mem)pSourceHandle, (cl_mem)pDestHandle, srcOffset, dstOffset, size, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to clEnqueueCopyBuffer! Code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking)
{
    int err = clEnqueueReadBuffer((cl_command_queue)m_pContext->GetOpenCLCommandQueue(), (cl_mem)pSourceHandle, (blocking)?CL_TRUE: CL_FALSE, srcOffset, size, (float*)pDest, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to clEnqueueReadBuffer! Code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking)
{
    int err = clEnqueueWriteBuffer((cl_command_queue)m_pContext->GetOpenCLCommandQueue(), (cl_mem)pDestHandle, (blocking)?CL_TRUE: CL_FALSE, dstOffset, size, (const float*)pSource, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to clEnqueueWriteBuffer! Code = %d\n", err);
        return AMF_FAIL;
    }
    return AMF_OK;
}

AMF_RESULT AMFDeviceOCLImpl::FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    int err = clEnqueueFillBuffer ((cl_command_queue)m_pContext->GetOpenCLCommandQueue(), (cl_mem)pDestHandle, pSourcePattern, patternSize, dstOffset, dstSize, 0, NULL, NULL);
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
    *ppHandle = clCreateBuffer((cl_context)m_pContext->GetOpenCLContext(), amf_to_cl_format(format), size, NULL, NULL);
    AMF_RETURN_IF_FALSE(*ppHandle != nullptr, AMF_OUT_OF_MEMORY, L"Error: Failed to allocate device memory!\n");
    return AMF_OK;
}
