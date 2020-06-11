#include "AMFFileTraceWriter.h"

#include <chrono>
#include <time.h>
#include "../../common/Thread.h"

amf::AMFCriticalSection      s_file_out_cs;

static wchar_t * timeWString()
{
	time_t rawtime;
	struct tm * timeinfo;

	wchar_t * buffer = new wchar_t[28];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	wcsftime(buffer, 21, L"%Y-%m-%d %T.", timeinfo);
	unsigned long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	int ms = now % 1000;
	wchar_t vOut[4];
	swprintf(vOut, 4, L"%d", ms);

	wcsncpy(&buffer[20], vOut, 4);
	return buffer;
}

void AMFFileTraceWriter::Write(const wchar_t * scope, const wchar_t * message)
{
	if (!m_fout.is_open())
		return;

	amf::AMFLock lock(&s_file_out_cs);
	wchar_t * timeInfo = timeWString();
	m_fout << timeInfo<< m_indentBuffer << " [" << scope << "] " << message << std::endl;
	free(timeInfo);
}

void AMFFileTraceWriter::Flush()
{
	m_fout.flush();
}

void AMFFileTraceWriter::updateIndentBuffer(amf_int32 indent)
{
	free(m_indentBuffer);
	m_indentBuffer = new wchar_t[indent + 1];
	wmemset(m_indentBuffer, ' ', indent);
	*(m_indentBuffer + indent) = '\0';
}

AMFFileTraceWriter::AMFFileTraceWriter()
{
	m_path = nullptr;
	m_indentBuffer = new wchar_t[1];
	m_indentBuffer[0] = '\0';
}

AMF_RESULT AMFFileTraceWriter::setPath(const wchar_t * path)
{
	amf::AMFLock lock(&s_file_out_cs);
	if (m_fout.is_open())
	{
		m_fout.flush();
		m_fout.close();
	}

	
#if defined(_WIN32)
	m_fout.open(path);
#else
	size_t size = wcstombs(nullptr, path, 0);
	char      *outputString = (char *)malloc(size * sizeof(char));
	wcstombs(outputString, path, size);

	m_fout.open(outputString);
	free(outputString);
#endif

	if (!m_fout.is_open())
	{
		return AMF_FILE_NOT_OPEN;
	}
	if (m_path)
		free(m_path);

	m_path = new wchar_t[wcslen(path) + 1];
	wcscpy(m_path, path);
	return AMF_OK;
}

AMF_RESULT AMFFileTraceWriter::GetPath(wchar_t * path, amf_size * pSize)
{
	if (!m_path)
		return AMF_FILE_NOT_OPEN;
	size_t size = wcslen(m_path);
	wcsncpy(path, m_path, size + 1);
	*pSize = size;
	return AMF_OK;
}

AMFFileTraceWriter::~AMFFileTraceWriter()
{
	if (m_fout.is_open())
	{
		m_fout.flush();
		m_fout.close();
	}
	if (m_path)
		free(m_path);

	free(m_indentBuffer);
}
