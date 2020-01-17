#include "FactoryImpl.h"
#include "ContextImpl.h"

#define AMF_FACILITY L"AMFFactoryImpl"

static AMFFactoryImpl g_AMFFactory;

AMFFactoryImpl::AMFFactoryImpl()
{

}


extern "C"
{
    AMF_CORE_LINK AMF_RESULT AMF_CDECL_CALL AMFInit(amf_uint64 version, amf::AMFFactory **ppFactory)
    {
        *ppFactory = &g_AMFFactory;
        return AMF_OK;
    }
}


AMF_RESULT AMFFactoryImpl::CreateContext(amf::AMFContext **ppContext)
{
    AMF_RETURN_IF_INVALID_POINTER(ppContext);
    AMF_RETURN_IF_FALSE((*ppContext = new AMFContextImpl) != NULL, AMF_OUT_OF_MEMORY);
    (*ppContext)->Acquire();
    return AMF_OK;
}

AMF_RESULT AMFFactoryImpl::CreateComponent(amf::AMFContext *pContext, const wchar_t *id, amf::AMFComponent **ppComponent)
{
    AMF_RETURN_IF_INVALID_POINTER(pContext);
    AMF_RETURN_IF_INVALID_POINTER(id);
    AMF_RETURN_IF_INVALID_POINTER(ppComponent);
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFFactoryImpl::SetCacheFolder(const wchar_t *path)
{
    return AMF_NOT_IMPLEMENTED;
}

const wchar_t *AMFFactoryImpl::GetCacheFolder()
{
    return NULL;
}

AMF_RESULT AMFFactoryImpl::GetDebug(amf::AMFDebug **ppDebug)
{
    AMF_RETURN_IF_INVALID_POINTER(ppDebug);
    *ppDebug = &m_Debug;
    return AMF_OK;
}

AMF_RESULT AMFFactoryImpl::GetTrace(amf::AMFTrace **ppTrace)
{
    AMF_RETURN_IF_INVALID_POINTER(ppTrace);
    *ppTrace = &m_Trace;
    return AMF_OK;
}

AMF_RESULT AMFFactoryImpl::GetPrograms(amf::AMFPrograms **ppPrograms)
{
    *ppPrograms = &m_Programs;
    return AMF_OK;
}
