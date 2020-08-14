#ifndef PROGRAMSIMPL_H
#define PROGRAMSIMPL_H

#include "../../include/core/Compute.h"
#include <vector>
#include <map>
#include <fstream>

using namespace amf;

class AMFKernelStorage
{
public:
	class KernelData
    {
	public:
		enum KernelType
		{
			Source,
			Binary
		};
        std::wstring kernelid_name;
        std::string kernelName;
        std::vector<amf_uint8> data;
        std::string options;
		KernelType type;

		std::string deviceName;
    };

    static AMFKernelStorage * Instance()
    {
        static AMFKernelStorage instance;
        return &instance;
    }

    void PushKernelData(const KernelData & data)
    {
        m_kernels.push_back(data);
    }

    AMF_KERNEL_ID KernelsCount() const
    {
        return m_kernels.size();
    }

    AMF_RESULT GetKernelData(KernelData** kernelData, AMF_KERNEL_ID kernelID)
    {
        if (kernelID < 0 || kernelID > m_kernels.size())
			return AMF_NOT_FOUND;

        *kernelData = &m_kernels[kernelID];
        return AMF_OK;
    }

	AMF_RESULT GetCacheKernelData(KernelData** kernelData,
		const wchar_t *kernelid_name,
		const char *kernelName,
		const char *deviceName,
		const char * extension)
	{
		amf_int64 kernelIndex = -1;
		AMF_KERNEL_ID i = 0;
		std::vector<KernelData>::iterator it;
		for (it = m_cachedKernels.begin(); it != m_cachedKernels.end(); it++, i++) {
			if (0 == wcscmp(it->kernelid_name.c_str(), kernelid_name) &&
				0 == strcmp(it->deviceName.c_str(), deviceName))
			{
				kernelIndex = i;
				break;
			}
		}

		if (kernelIndex < 0)
		{
			if (!m_cacheFolder.length())
				return AMF_FAIL;

			auto resultName = ResultFileName(kernelid_name, deviceName, extension);

			FILE *fp = fopen(resultName.c_str(), "rb");
			if (fp == NULL)
			{
				return AMF_FAIL;
			}

			fseek(fp, 0, SEEK_END);

			AMFKernelStorage::KernelData kernelData = {
				kernelid_name,
				kernelName,
				std::vector<amf_uint8>(ftell(fp)), //alloc vector with ftell size
				"",
				KernelData::Binary,
				deviceName
				};

			if(kernelData.data.size() > 0)
			{
				rewind(fp);
				fread(&kernelData.data.front(), 1, kernelData.data.size(), fp);
			}

			fclose(fp);

			kernelIndex = m_cachedKernels.size();
			m_cachedKernels.push_back(kernelData);
		}

		*kernelData = &m_cachedKernels[kernelIndex];

		return AMF_OK;
	}

	std::string ResultFileName(const std::wstring & name, const std::string & deviceName, const std::string & extension)
	{
		size_t size = wcstombs(nullptr, name.c_str(), 0);
		std::string nameInChars(size, 0);
		wcstombs(&nameInChars.front(), name.c_str(), size);

		size = wcstombs(nullptr, m_cacheFolder.c_str(), 0);
		std::string folderInChars(size, 0);
		wcstombs(&folderInChars.front(), m_cacheFolder.c_str(), size);

		size = wcstombs(nullptr, PATH_SEPARATOR_WSTR, 0);
		std::string delimiter(size, 0);
		wcstombs(&delimiter.front(), PATH_SEPARATOR_WSTR, size);

		return folderInChars
			+ delimiter
			+ nameInChars
			+ extension;
	}

	AMF_RESULT SaveProgramBinary(
		const std::wstring & 	name,
		const std::string & 	kernelName,
		const std::string & 	deviceName,
		const std::string & 	extension,
		const amf_uint8 *		data,
		size_t					dataSize
		)
	{
		auto resultName = ResultFileName(name, deviceName, extension);

		FILE *fp = fopen(resultName.c_str(), "wb");
		if (!fp)
			return AMF_FAIL;
		fwrite(data, 1,	dataSize, fp);
		fclose(fp);

		amf_int64 kernelIndex = -1;
		AMF_KERNEL_ID i = 0;

		std::vector<KernelData>::iterator it;
		for (it = m_cachedKernels.begin(); it != m_cachedKernels.end(); it++, i++) {
			if (0 == it->kernelid_name.compare(name) &&
				0 == it->deviceName.compare(deviceName))
			{
				kernelIndex = i;
				break;
			}
		}

		if (kernelIndex < 0)
		{
			AMFKernelStorage::KernelData kernelData = {
				name,
				kernelName,
				std::vector<amf_uint8>(dataSize),
				"",
				KernelData::Binary,
				deviceName
				};
			std::copy(data, data + dataSize, std::back_inserter(kernelData.data));

			m_cachedKernels.push_back(kernelData);
		}
		else
		{
			m_cachedKernels[kernelIndex].data.resize(dataSize);
			std::copy(data, data + dataSize, std::back_inserter(m_cachedKernels[kernelIndex].data));
		}

		return AMF_OK;
	}

	AMF_RESULT SetCacheFolder(const wchar_t *path)
	{
		m_cacheFolder = path;

		return AMF_OK;
	}

	wchar_t * GetCacheFolder()
	{
		return &m_cacheFolder.front();
	}

	amf_int64 FindSourceIndex(const wchar_t *kernelid_name, const char *options, KernelData::KernelType type = KernelData::Source)
	{
		amf_int64 i = 0;
		std::vector<KernelData>::iterator it;
		for (it = m_kernels.begin(); it != m_kernels.end(); it++, i++) {
			if (it->type != type)
				continue;
			if (0 == wcscmp(it->kernelid_name.c_str(), kernelid_name))
			{
				if (!options && !it->options.length())
					return i;
				if (0 == strcmp(it->options.c_str(), options))
					return i;
			}
		}

		return -1;
	}

private:
	AMFKernelStorage() {}
	virtual ~AMFKernelStorage()
	{
		//TODO: cleanup kernels
	}

	std::wstring m_cacheFolder;
    std::vector<KernelData> m_kernels;
	std::vector<KernelData> m_cachedKernels;
};


class AMFProgramsImpl : public AMFPrograms
{
public:
    AMFProgramsImpl();

    // AMFPrograms interface
public:
    virtual AMF_RESULT AMF_STD_CALL RegisterKernelSourceFile(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, const wchar_t *filepath, const char *options) override;
    virtual AMF_RESULT AMF_STD_CALL RegisterKernelSource(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
    virtual AMF_RESULT AMF_STD_CALL RegisterKernelBinary(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
    virtual AMF_RESULT AMF_STD_CALL RegisterKernelSource1(AMF_MEMORY_TYPE eMemoryType, AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
    virtual AMF_RESULT AMF_STD_CALL RegisterKernelBinary1(AMF_MEMORY_TYPE eMemoryType, AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
};

#endif // PROGRAMSIMPL_H
