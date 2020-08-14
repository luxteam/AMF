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
	AMF_KERNEL_ID kernelId = AMFKernelStorage::Instance()->FindSourceIndex(
    	kernelid_name,
		options
		);

	if (kernelId != -1)
	{
		*pKernelID = kernelId;

		return AMF_OK;
	}

	AMFKernelStorage::KernelData kernelData = {
		kernelid_name,
		kernelName,
		std::vector<amf_uint8>(data, data + dataSize),
		options ? options : std::string(),
		AMFKernelStorage::KernelData::Source
	    };

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
