#ifndef DEVICEIMPL_H
#define DEVICEIMPL_H

#include "Device.h"
#include "../../common/InterfaceImpl.h"

using namespace amf;
class AMFContextImpl;

class AMFDeviceImpl : public AMFInterfaceImpl<AMFDevice>
{
public:
    AMFDeviceImpl(AMF_MEMORY_TYPE type, amf_size cacheSize, AMFContextImpl* pContext);

public:
    virtual AMF_MEMORY_TYPE GetType() override;

    virtual AMF_RESULT AllocateBuffer(amf_size size, void **ppHandle) override;
    virtual AMF_RESULT AttachBuffer(amf_size size, void *pHandle) override;
    virtual AMF_RESULT ReleaseBuffer(void *pHandle, bool attached) override;

    virtual AMF_RESULT CopyBuffer(void *pDestHandle, amf_size dstOffset, void *pSourceHandle, amf_size srcOffset, amf_size size) override;
    virtual AMF_RESULT CopyBufferToHost(void *pDest, void *pSourceHandle, amf_size srcOffset, amf_size size, bool blocking) override;
    virtual AMF_RESULT CopyBufferFromHost(void *pDestHandle, amf_size dstOffset, const void *pSource, amf_size size, bool blocking) override;
    virtual AMF_RESULT FillBuffer(void *pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize) override;
private:
    AMF_MEMORY_TYPE m_type;
    amf_size        m_cacheSize;
protected:
    AMFContextImpl* m_pContext;

    // AMFDevice interface
};

template <class Base>
class AMFDeviceImpl_T : public AMFInterfaceImpl<Base>, public AMFDeviceImpl
{
public:
    AMFDeviceImpl_T(AMF_MEMORY_TYPE type, amf_size cacheSize, AMFContextImpl* pContext)
        :AMFDeviceImpl(type, cacheSize, pContext)
    {
    }

    // interface access
    AMF_BEGIN_INTERFACE_MAP
        AMF_INTERFACE_CHAIN_ENTRY(AMFInterfaceImpl<Base>)
        AMF_INTERFACE_ENTRY(AMFDevice)
    AMF_END_INTERFACE_MAP
};

#endif // DEVICEIMPL_H
