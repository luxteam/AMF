#ifndef AMFCOMPUTEKERNELMETAL_H
#define AMFCOMPUTEKERNELMETAL_H

#include "../../../include/core/Compute.h"
#include "../../../include/core/ComputeFactory.h"
#include "../../../common/InterfaceImpl.h"
#include "../../../common/AMFSTL.h"
#include "../../../common/TraceAdapter.h"
#include "../../../common/PropertyStorageImpl.h"

using namespace amf;
class MetalComputeKernel;

class AMFComputeKernelMetal : public AMFInterfaceImpl<AMFComputeKernel>
{
public:
    AMFComputeKernelMetal(AMF_KERNEL_ID kernelID, void * kernel);
	~AMFComputeKernelMetal();
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
    MetalComputeKernel *m_kernel;
};

#endif // AMFCOMPUTEKERNELMETAL_H
