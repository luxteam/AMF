#include "ProgramsImpl.h"

AMFProgramsImpl::AMFProgramsImpl()
{

}


AMF_RESULT AMFProgramsImpl::RegisterKernelSourceFile(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, const wchar_t *filepath, const char *options)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFProgramsImpl::RegisterKernelSource(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options)
{
    AMFKernelStorage::KernelData kernelData;
    kernelData.kernelid_name = kernelid_name;
    kernelData.kernelName = kernelName;
    kernelData.dataSize = dataSize + 1;
    kernelData.data = new amf_uint8 [kernelData.dataSize];
    memcpy((void*)kernelData.data, data, kernelData.dataSize);

    kernelData.options = options;
    AMFKernelStorage::Instance()->PushKernelData(kernelData);
    *pKernelID = AMFKernelStorage::Instance()->KernelsCount() - 1;
    return AMF_OK;
}

AMF_RESULT AMFProgramsImpl::RegisterKernelBinary(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFProgramsImpl::RegisterKernelSource1(AMF_MEMORY_TYPE eMemoryType, AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFProgramsImpl::RegisterKernelBinary1(AMF_MEMORY_TYPE eMemoryType, AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options)
{
    return AMF_NOT_IMPLEMENTED;
}