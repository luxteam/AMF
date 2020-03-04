#ifndef AMFFILETRACEWRITER_H
#define AMFFILETRACEWRITER_H

#include "../../include/core/Trace.h"
#include "../../include/core/Data.h"
#include <fstream>

class AMFFileTraceWriter : public amf::AMFTraceWriter
{
public:
	AMFFileTraceWriter();

	AMF_RESULT setPath(const wchar_t * path);

	AMF_RESULT GetPath(wchar_t *path, amf_size *pSize);

	virtual ~AMFFileTraceWriter();
	virtual void AMF_CDECL_CALL Write(const wchar_t* scope, const wchar_t* message) override;
	virtual void AMF_CDECL_CALL Flush() override;

	void updateIndentBuffer(amf_int32 indent);
private:
	std::wofstream m_fout;
	wchar_t * m_path;
	wchar_t * m_indentBuffer;
};

#endif// AMFFILETRACEWRITER_H