#ifndef AMFDEBUGIMPL_H
#define AMFDEBUGIMPL_H
#include "../../include/core/Trace.h"
#include "../../include/core/Debug.h"

using namespace amf;

class AMFDebugImpl : public AMFDebug
{
public:
    AMFDebugImpl();

    // AMFDebug interface
public:
    virtual void EnablePerformanceMonitor(amf_bool enable) override;
    virtual amf_bool PerformanceMonitorEnabled() override;
    virtual void AssertsEnable(amf_bool enable) override;
    virtual amf_bool AssertsEnabled() override;
};

#endif // AMFDEBUGIMPL_H
