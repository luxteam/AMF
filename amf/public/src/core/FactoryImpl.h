#ifndef AMFFACTORYIMPL_H
#define AMFFACTORYIMPL_H

#include "../../common/TraceAdapter.h"
#include "../../include/core/Factory.h"
#include "ProgramsImpl.h"
#include "DebugImpl.h"
#include "TraceImpl.h"

class AMFFactoryImpl : public amf::AMFFactory
{
public:
    AMFFactoryImpl();

    // AMFFactory interface
public:
    virtual AMF_RESULT AMF_STD_CALL CreateContext(amf::AMFContext **ppContext) override;
    virtual AMF_RESULT AMF_STD_CALL CreateComponent(amf::AMFContext *pContext, const wchar_t *id, amf::AMFComponent **ppComponent) override;
    virtual AMF_RESULT AMF_STD_CALL SetCacheFolder(const wchar_t *path) override;
    virtual const wchar_t * AMF_STD_CALL GetCacheFolder() override;
    virtual AMF_RESULT AMF_STD_CALL GetDebug(amf::AMFDebug **ppDebug) override;
    virtual AMF_RESULT AMF_STD_CALL GetTrace(amf::AMFTrace **ppTrace) override;
    virtual AMF_RESULT AMF_STD_CALL GetPrograms(amf::AMFPrograms **ppPrograms) override;

private:
    AMFProgramsImpl m_Programs;
    AMFTraceImpl m_Trace;
    AMFDebugImpl m_Debug;
};

#endif // AMFFACTORYIMPL_H
