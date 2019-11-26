#ifndef DEVICEOCLIMPL_H
#define DEVICEOCLIMPL_H
#include "DeviceImpl.h"
#include "ContextImpl.h"

using namespace amf;

class AMFDeviceOCLImpl : public AMFDeviceImpl
{
public:
    AMFDeviceOCLImpl(AMFContextImpl* pContext);

    // AMFDevice interface
public:
    virtual AMF_MEMORY_TYPE GetType() override;

    virtual AMF_RESULT AllocateBuffer(amf_size size, void **ppHandle) override;
    virtual AMF_RESULT AttachBuffer(amf_size size, void *pHandle) override;
    virtual AMF_RESULT ReleaseBuffer(void *pHandle, bool attached) override;

    virtual AMF_RESULT CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size) override;
    virtual AMF_RESULT CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking) override;
    virtual AMF_RESULT CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking) override;
    virtual AMF_RESULT FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize) override;

    //
    AMF_RESULT AllocateBufferEx(amf_size size, void **ppHandle, enum AMF_ARGUMENT_ACCESS_TYPE format);
};

#endif // DEVICEOCLIMPL_H
