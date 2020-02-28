#ifndef AMFCONSOLETRACEWRITER_H
#define AMFCONSOLETRACEWRITER_H

#include <string>
#include <memory>
#include <sstream>
#include <ios>
#include "../../common/Thread.h"
#include "../../include/core/Result.h"
#include "../../include/core/Trace.h"
#include "../../include/core/Data.h"

class AMFConsoleTraceWriter : public amf::AMFTraceWriter
{
public:
    AMFConsoleTraceWriter()
    {
    }
    virtual ~AMFConsoleTraceWriter()
    {
    }
	virtual void AMF_CDECL_CALL Write(const wchar_t* scope, const wchar_t* message) override;
	void AMF_CDECL_CALL Write(const wchar_t* scope, const wchar_t* message, amf_int32 level);
    virtual void AMF_CDECL_CALL Flush() override
    {
    }
};
#endif // AMFCONSOLETRACEWRITER_H
