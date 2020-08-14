#include "AMFComputeFactoryMetal.h"

AMFComputeFactoryMetal::AMFComputeFactoryMetal(AMFContextImpl* pContext)
:m_pContext(pContext)
{
    m_wrapper.reset(new MetalDeviceEnumeratorWrapper());
}

AMFComputeFactoryMetal::~AMFComputeFactoryMetal()
{
}

AMF_RESULT AMFComputeFactoryMetal::Init()
{
    m_wrapper->init();

    for(int i = 0; i < m_wrapper->GetDeviceCount(); ++i)
    {
        AMFDeviceMetalImpl* deviceImpl = new AMFDeviceMetalImpl(
            m_pContext,
            m_wrapper->GetDeviceAt(i),
            m_wrapper->GetDeviceName(i)
            );

        m_devices.push_back(deviceImpl);
    }

    return AMF_OK;
}

char * AMFComputeFactoryMetal::GetDeviceName(int index) const
{
    return m_wrapper->GetDeviceName(index);
}

amf_int32 AMFComputeFactoryMetal::GetDeviceCount()
{
    return m_devices.size();
}

AMF_RESULT AMFComputeFactoryMetal::GetDeviceAt(amf_int32 index, AMFComputeDevice **ppDevice)
{
    //TODO: check out of range
    AMFDeviceMetalImpl* deviceImpl = dynamic_cast<AMFDeviceMetalImpl*>(m_devices.at(index).GetPtr());
    *ppDevice = deviceImpl->GetComputeDevice();
    (*ppDevice)->Acquire();
    return AMF_OK;
}