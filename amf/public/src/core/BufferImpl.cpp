#include "BufferImpl.h"
#include "DeviceOCLImpl.h"

typedef AMFInterfacePtr_T<AMFBufferImpl> AMFBufferImplPtr;

AMFBufferImpl::AMFBufferImpl(AMFContextImpl *pContext)
    :AMFDataImpl(pContext),
        m_pMemory(nullptr),
        m_size(0),
        m_attached(false)
{
}

AMF_MEMORY_TYPE AMFBufferImpl::GetMemoryType()
{
    auto implementation(dynamic_cast<AMFDeviceOCLImpl *>(m_pDevice.GetPtr()));

    return /*m_pDevice*/implementation->GetType();
}

AMF_RESULT AMFBufferImpl::Duplicate(AMF_MEMORY_TYPE type, AMFData **ppData)
{
    AMFBufferImplPtr pBuffer;
    AMF_RETURN_IF_FAILED(Duplicate(type, &pBuffer));
    pBuffer->QueryInterface(AMFData::IID(), (void**)ppData);
    return AMF_OK;
}

AMF_RESULT AMFBufferImpl::Duplicate(AMF_MEMORY_TYPE type, AMFBufferImpl **ppData)
{
    AMF_RETURN_IF_FALSE(GetContext()->GetDevice(type) != nullptr, AMF_NO_DEVICE);
    AMF_RETURN_IF_FALSE(ppData != nullptr, AMF_INVALID_POINTER);

    if((type != AMF_MEMORY_HOST) && (GetMemoryType() != AMF_MEMORY_HOST))
    {
        AMFBufferImplPtr pHostData;
        AMF_RETURN_IF_FAILED(Duplicate(AMF_MEMORY_HOST, &pHostData));
        AMF_RETURN_IF_FAILED(pHostData->Duplicate(type, ppData));
        return AMF_OK;
    }

    AMFBufferImplPtr pBuffer = new AMFBufferImpl(GetContext());

    AMF_RETURN_IF_FAILED(pBuffer->Allocate(type, GetSize()));

    if((type == AMF_MEMORY_HOST) && (GetMemoryType() == AMF_MEMORY_HOST))
    {
        //From Host to Host
        AMFComputeDevicePtr pDevice = GetContext()->GetDevice(type);

        auto deviceImplementation = dynamic_cast<AMFDeviceOCLImpl *>(pDevice.GetPtr());
        AMF_RETURN_IF_FAILED(/*pDevice*/deviceImplementation->CopyBuffer(pBuffer->m_pMemory, 0, m_pMemory, 0, GetSize()));
    }
    else
    {
        if(GetMemoryType() == AMF_MEMORY_HOST)
        {
            AMFComputeDevicePtr pDevice = GetContext()->GetDevice(type);

            auto deviceImplementation = dynamic_cast<AMFDeviceOCLImpl *>(pDevice.GetPtr());
            AMF_RETURN_IF_FAILED(/*pDevice*/deviceImplementation->CopyBufferFromHost(pBuffer->m_pMemory, 0, m_pMemory, GetSize(), true));
        }
        else
        {
            AMFComputeDevicePtr pDevice = GetContext()->GetDevice(GetMemoryType());

            auto deviceImplementation = dynamic_cast<AMFDeviceOCLImpl *>(pDevice.GetPtr());
            AMF_RETURN_IF_FAILED(/*pDevice*/deviceImplementation->CopyBufferToHost(pBuffer->m_pMemory, m_pMemory, 0, GetSize(), true));
        }
    }

    pBuffer->SetPts(GetPts());
    pBuffer->SetDuration(GetDuration());

    *ppData = pBuffer;
    (*ppData)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFBufferImpl::Convert(AMF_MEMORY_TYPE type)
{
    AMF_RETURN_IF_FALSE(GetContext()->GetDevice(type) != nullptr, AMF_NO_DEVICE);
    AMF_RETURN_IF_FALSE(m_pMemory != nullptr, AMF_NOT_INITIALIZED);

    if(GetMemoryType() == type)
        return AMF_OK;

    AMFBufferImplPtr pTmpBuffer;
    AMF_RETURN_IF_FAILED(Duplicate(type, &pTmpBuffer));

    AMFObservableImpl<AMFBufferObserver>::NotifyObservers<AMFBuffer*>(&AMFBufferObserver::OnBufferDataRelease, this);

    auto implementation(dynamic_cast<AMFDeviceOCLImpl *>(m_pDevice.GetPtr()));
    /*m_pDevice*/implementation->ReleaseBuffer(m_pMemory, m_attached);
    m_attached = false;
    m_pMemory = pTmpBuffer->m_pMemory;
    pTmpBuffer->m_pMemory = nullptr;
    m_pDevice = GetContext()->GetDevice(type);
    return AMF_OK;
}

AMF_RESULT AMFBufferImpl::Interop(AMF_MEMORY_TYPE type)
{
    return AMF_NOT_SUPPORTED;
}

AMF_DATA_TYPE AMFBufferImpl::GetDataType()
{
    return AMF_DATA_BUFFER;
}

AMF_RESULT AMFBufferImpl::SetSize(amf_size newSize)
{
    m_size = newSize;
    return AMF_OK;
}

amf_size AMFBufferImpl::GetSize()
{
    return m_size;
}

void *AMFBufferImpl::GetNative()
{
    return m_pMemory;
}

AMF_RESULT AMFBufferImpl::Allocate(AMF_MEMORY_TYPE type, amf_size size)
{
    AMFDeviceOCLImpl* pDevice = dynamic_cast<AMFDeviceOCLImpl *>(GetContext()->GetDevice(type));

    AMF_RETURN_IF_FALSE(pDevice != nullptr, AMF_NO_DEVICE);
    AMF_RETURN_IF_FALSE(m_pMemory == nullptr, AMF_ALREADY_INITIALIZED);
    AMF_RETURN_IF_FALSE(size > 0, AMF_INVALID_ARG);

    void* pMemory = nullptr;
    AMF_RETURN_IF_FAILED(pDevice->AllocateBuffer(size, &pMemory));

    m_pDevice = pDevice;
    m_pMemory = pMemory;
    m_attached = false;
    m_size = size;
    return AMF_OK;
}

AMF_RESULT AMFBufferImpl::Attach(AMF_MEMORY_TYPE type, void *pNative, amf_size size)
{
    AMFComputeDevice* pDevice = nullptr;

    AMF_RETURN_IF_FALSE((pDevice = GetContext()->GetDevice(type)) != nullptr, AMF_NO_DEVICE);
    AMF_RETURN_IF_FALSE(m_pMemory == nullptr, AMF_ALREADY_INITIALIZED);
    AMF_RETURN_IF_FALSE(size > 0, AMF_INVALID_ARG);

    m_attached = true;
    m_pDevice = pDevice;
    m_pMemory = pNative;

    auto implementation(dynamic_cast<AMFDeviceOCLImpl *>(pDevice));
    /*pDevice*/implementation->AttachBuffer(size, m_pMemory);
    m_size = size;
    return AMF_OK;
}