#include "BufferImpl.h"
#include "DeviceHostImpl.h"
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
    return m_pDevice->GetType();
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
        AMFDevice *pDevice = GetContext()->GetDevice(type);
        AMF_RETURN_IF_FAILED(pDevice->CopyBuffer(pBuffer->m_pMemory, 0, m_pMemory, 0, GetSize()));
    }
    else
    {
        if(GetMemoryType() == AMF_MEMORY_HOST)
        {
            AMFDevice *pDevice = GetContext()->GetDevice(type);
            AMF_RETURN_IF_FAILED(pDevice->CopyBufferFromHost(pBuffer->m_pMemory, 0, m_pMemory, GetSize(), true));
        }
        else
        {
            AMFDevice *pDevice = GetContext()->GetDevice(GetMemoryType());
            AMF_RETURN_IF_FAILED(pDevice->CopyBufferToHost(pBuffer->m_pMemory, m_pMemory, 0, GetSize(), true));
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

    m_pDevice->ReleaseBuffer(m_pMemory, m_attached);
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
    AMFDeviceOCLImpl *p1(type == AMF_MEMORY_TYPE::AMF_MEMORY_OPENCL ? GetContext()->GetDeviceOCL() : nullptr);
    AMFDeviceHostImpl *p2(type != AMF_MEMORY_TYPE::AMF_MEMORY_OPENCL ? GetContext()->GetDeviceHost() : nullptr);

    //AMFDevice* pDevice = (AMFDevice *)GetContext()->GetDevice(type);
    //void* pMemory = nullptr;
    alignas(32) void* pMemory = nullptr;

    AMF_RETURN_IF_FALSE(p1 != nullptr || p2 != nullptr, AMF_NO_DEVICE);
    AMF_RETURN_IF_FALSE(m_pMemory == nullptr, AMF_ALREADY_INITIALIZED);
    AMF_RETURN_IF_FALSE(size > 0, AMF_INVALID_ARG);

    //auto test(dynamic_cast<AMFDeviceOCLImpl *>(pDevice));
    //AMF_RETURN_IF_FALSE(test != nullptr, AMF_FAIL);

    auto pointer = &pMemory;

    AMF_RETURN_IF_FAILED(
        type == AMF_MEMORY_TYPE::AMF_MEMORY_OPENCL
            ? p1->AllocateBuffer(size, pointer)
            : p2->AllocateBuffer(size, pointer)
        );

    m_pDevice = p1 ? (AMFDevice *)p1 : (AMFDevice *)p2;
    m_pMemory = pMemory;
    m_attached = false;
    m_size = size;
    return AMF_OK;
}

AMF_RESULT AMFBufferImpl::Attach(AMF_MEMORY_TYPE type, void *pNative, amf_size size)
{
    AMFDevice* pDevice = (AMFDevice *)GetContext()->GetDevice(type);

    AMF_RETURN_IF_FALSE(pDevice != nullptr, AMF_NO_DEVICE);
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