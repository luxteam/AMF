#include "AMFConsoleTraceWriter.h"
#include <iostream>
#include <iomanip>

#ifdef _WIN32
void ChangeTextColor(amf_int32 level)
{
#if !defined(METRO_APP)
    HANDLE hCmd = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (level)
    {
    case AMF_TRACE_WARNING:
        SetConsoleTextAttribute(hCmd, FOREGROUND_GREEN);
        break;
    case AMF_TRACE_ERROR:
        SetConsoleTextAttribute(hCmd, FOREGROUND_RED);
        break;
	default:
		SetConsoleTextAttribute(hCmd, FOREGROUND_INTENSITY);
		break;
    }
#endif
}
#elif defined(__linux)
void ChangeTextColor(amf_int32 level)
{
	switch (level)
	{
	case AMF_TRACE_WARNING:
		wprintf(L"\033[22;32m");
		break;
	case AMF_TRACE_ERROR:
		wprintf(L"\033[22;31m");
		break;
	default:
		wprintf(L"\033[0m");
		break;
	}
}
#elif defined(__APPLE__)
void ChangeTextColor(amf_int32 level)
{
	switch (level)
	{
	case AMF_TRACE_WARNING:
		wprintf(L"\x1b[32m");
		break;
	case AMF_TRACE_ERROR:
		wprintf(L"\x1b[31m");
		break;
	default:
		wprintf(L"\x1b[0m");
		break;
	}
}
#else
void ChangeTextColor(AMFLogLevel level)
{
}
#endif

amf::AMFCriticalSection      s_std_out_cs;

void AMFConsoleTraceWriter::Write(const wchar_t * scope, const wchar_t * message)
{
	/*amf::AMFLock lock(&s_std_out_cs);
	wprintf(message);
	wprintf(L"\n");*/
}

void AMFConsoleTraceWriter::Write(const wchar_t * scope, const wchar_t * message, amf_int32 level)
{
	amf::AMFLock lock(&s_std_out_cs);
	ChangeTextColor(level);
	wprintf(message);
	wprintf(L"\n");
	ChangeTextColor(AMF_TRACE_INFO);
}
