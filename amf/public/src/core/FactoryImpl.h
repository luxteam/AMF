#ifndef AMFFACTORYIMPL_H
#define AMFFACTORYIMPL_H

#include "../../common/TraceAdapter.h"
#include "../../include/core/Factory.h"
#include "ProgramsImpl.h"

class AMFFactoryImpl : public amf::AMFFactory
{
public:
    AMFFactoryImpl();

    // AMFFactory interface
public:
    virtual AMF_RESULT CreateContext(amf::AMFContext **ppContext) override;
    virtual AMF_RESULT CreateComponent(amf::AMFContext *pContext, const wchar_t *id, amf::AMFComponent **ppComponent) override;
    virtual AMF_RESULT SetCacheFolder(const wchar_t *path) override;
    virtual const wchar_t *GetCacheFolder() override;
    virtual AMF_RESULT GetDebug(amf::AMFDebug **ppDebug) override;
    virtual AMF_RESULT GetTrace(amf::AMFTrace **ppTrace) override;
    virtual AMF_RESULT GetPrograms(amf::AMFPrograms **ppPrograms) override;

private:
    AMFProgramsImpl m_Programs;
};

#endif // AMFFACTORYIMPL_H
