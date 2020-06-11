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
	AMF_KERNEL_ID kernelId = AMFKernelStorage::Instance()->FindSourceIndex(kernelid_name, options);
	if (kernelId != -1)
	{
		*pKernelID = kernelId;
		return AMF_OK;
	}

	size_t size = wcslen(kernelid_name) + 1;

    AMFKernelStorage::KernelData kernelData;
    kernelData.kernelid_name = (wchar_t *)malloc(size * sizeof(wchar_t));
	wcsncpy(kernelData.kernelid_name, kernelid_name, size);

	size = strlen(kernelName) + 1;
    kernelData.kernelName = (char *)malloc(size * sizeof(char));
	strcpy(kernelData.kernelName, kernelName);

    kernelData.dataSize = dataSize + 1;
    kernelData.data = new amf_uint8 [kernelData.dataSize];
    memcpy((void*)kernelData.data, data, kernelData.dataSize);
	kernelData.type = kernelData.Source;

	if (options != NULL)
	{
		size = strlen(kernelName) + 1;
		kernelData.options = (char *)malloc(size * sizeof(char));
		strcpy(kernelData.options, options);
	}

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
