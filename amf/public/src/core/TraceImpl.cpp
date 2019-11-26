#include "TraceImpl.h"

AMFTraceImpl::AMFTraceImpl()
{

}

AMF_AUDIO_FORMAT AMFTraceImpl::GetSampleFormatByName(const wchar_t *name)
{
    return AMFAF_UNKNOWN;
}

const wchar_t * const AMFTraceImpl::GetSampleFormatName(const AMF_AUDIO_FORMAT eFormat)
{
    return L"AMFAF_UNKNOWN";
}

AMF_MEMORY_TYPE AMFTraceImpl::GetMemoryTypeByName(const wchar_t *name)
{
    return AMF_MEMORY_UNKNOWN;
}

const wchar_t * const AMFTraceImpl::GetMemoryTypeName(const AMF_MEMORY_TYPE memoryType)
{
    return L"AMF_MEMORY_UNKNOWN";
}

AMF_SURFACE_FORMAT AMFTraceImpl::SurfaceGetFormatByName(const wchar_t *name)
{
    return AMF_SURFACE_UNKNOWN;
}

const wchar_t *AMFTraceImpl::SurfaceGetFormatName(const AMF_SURFACE_FORMAT eSurfaceFormat)
{
    return L"AMF_SURFACE_UNKNOWN";
}

const wchar_t *AMFTraceImpl::GetResultText(AMF_RESULT res)
{
    return L"";
}

void AMFTraceImpl::UnregisterWriter(const wchar_t *writerID)
{

}

void AMFTraceImpl::RegisterWriter(const wchar_t *writerID, AMFTraceWriter *pWriter, amf_bool enable)
{

}

void AMFTraceImpl::Indent(amf_int32 addIndent)
{

}

amf_int32 AMFTraceImpl::GetIndentation()
{
    return 0;
}

amf_int32 AMFTraceImpl::GetWriterLevelForScope(const wchar_t *writerID, const wchar_t *scope)
{
    return 0;
}

amf_int32 AMFTraceImpl::SetWriterLevelForScope(const wchar_t *writerID, const wchar_t *scope, amf_int32 level)
{
    return 0;
}

amf_int32 AMFTraceImpl::GetWriterLevel(const wchar_t *writerID)
{
    return 0;
}

amf_int32 AMFTraceImpl::SetWriterLevel(const wchar_t *writerID, amf_int32 level)
{
    return 0;
}

AMF_RESULT AMFTraceImpl::GetPath(wchar_t *path, amf_size *pSize)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFTraceImpl::SetPath(const wchar_t *path)
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFTraceImpl::TraceFlush()
{
    return AMF_NOT_IMPLEMENTED;
}

AMF_RESULT AMFTraceImpl::TraceEnableAsync(amf_bool enable)
{
    return AMF_NOT_IMPLEMENTED;
}

amf_bool AMFTraceImpl::WriterEnabled(const wchar_t *writerID)
{
    return false;
}

amf_bool AMFTraceImpl::EnableWriter(const wchar_t *writerID, bool enable)
{
    return false;
}

amf_int32 AMFTraceImpl::GetGlobalLevel()
{
    return 0;
}

amf_int32 AMFTraceImpl::SetGlobalLevel(amf_int32 level)
{
    return 0;
}

void AMFTraceImpl::Trace(const wchar_t *src_path, amf_int32 line, amf_int32 level, const wchar_t *scope, const wchar_t *message, va_list *pArglist)
{

}

void AMFTraceImpl::TraceW(const wchar_t *src_path, amf_int32 line, amf_int32 level, const wchar_t *scope, amf_int32 countArgs, const wchar_t *format, ...)
{

}
