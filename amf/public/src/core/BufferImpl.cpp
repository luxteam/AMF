#include "BufferImpl.h"

typedef AMFInterfacePtr_T<AMFBufferImpl> AMFBufferImplPtr;

AMFBufferImpl::AMFBufferImpl(AMFContextImpl *pContext)
    :AMFDataImpl(pContext),
        m_pMemory(nullptr),
        m_size(0),
        m_attached(false)
{
}

inline AMFBufferImpl::~AMFBufferImpl()
{
	if (m_pDevice && m_pMemory)
		m_pDevice->ReleaseBuffer(m_pMemory, m_attached);
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
        AMFDevicePtr pDevice = GetContext()->GetDevice(type);
        AMF_RETURN_IF_FAILED(pDevice->CopyBuffer(pBuffer->m_pMemory, 0, m_pMemory, 0, GetSize()));
    }
    else
    {
        if(GetMemoryType() == AMF_MEMORY_HOST)
        {
            AMFDevicePtr pDevice = GetContext()->GetDevice(type);
            AMF_RETURN_IF_FAILED(pDevice->CopyBufferFromHost(pBuffer->m_pMemory, 0, m_pMemory, GetSize(), true));
        }
        else
        {
            AMFDevicePtr pDevice = GetContext()->GetDevice(GetMemoryType());
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

AMF_RESULT AMF_STD_CALL AMFBufferImpl::CreateSubBuffer(AMFBuffer** subBuffer, amf_size offset, amf_size size)
{
	AMFDevice* pDevice = nullptr;
	void* pMemory = nullptr;

	AMF_RETURN_IF_FALSE((pDevice = GetContext()->GetDevice(GetMemoryType())) != nullptr, AMF_NO_DEVICE);

	AMF_RETURN_IF_FAILED(pDevice->CreateSubBuffer(this, &pMemory, offset, size));

	*subBuffer = new AMFBufferImpl(GetContext());

	((AMFBufferImpl *)(*subBuffer))->m_pDevice = pDevice;
	((AMFBufferImpl *)(*subBuffer))->m_pMemory = pMemory;
	((AMFBufferImpl *)(*subBuffer))->m_attached = false;
	((AMFBufferImpl *)(*subBuffer))->m_size = m_size;

	return AMF_OK;
}

AMF_RESULT AMFBufferImpl::Allocate(AMF_MEMORY_TYPE type, amf_size size)
{
    AMFDevice* pDevice = nullptr;
    void* pMemory = nullptr;

    AMF_RETURN_IF_FALSE((pDevice = GetContext()->GetDevice(type)) != nullptr, AMF_NO_DEVICE);
    AMF_RETURN_IF_FALSE(m_pMemory == nullptr, AMF_ALREADY_INITIALIZED);
    AMF_RETURN_IF_FALSE(size > 0, AMF_INVALID_ARG);

    AMF_RETURN_IF_FAILED(pDevice->AllocateBuffer(size, &pMemory));

    m_pDevice = pDevice;
    m_pMemory = pMemory;
    m_attached = false;
    m_size = size;
    return AMF_OK;
}

AMF_RESULT AMFBufferImpl::Attach(AMF_MEMORY_TYPE type, void *pNative, amf_size size)
{
    AMFDevice* pDevice = nullptr;

    AMF_RETURN_IF_FALSE((pDevice = GetContext()->GetDevice(type)) != nullptr, AMF_NO_DEVICE);
    AMF_RETURN_IF_FALSE(m_pMemory == nullptr, AMF_ALREADY_INITIALIZED);
    AMF_RETURN_IF_FALSE(size > 0, AMF_INVALID_ARG);

    m_attached = true;
    m_pDevice = pDevice;
    m_pMemory = pNative;
    pDevice->AttachBuffer(size, m_pMemory);
    m_size = size;
    return AMF_OK;
}
