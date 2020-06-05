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
        wchar_t *kernelid_name;
        char *kernelName;
        amf_size dataSize;
		amf_uint8 *data = nullptr;
        char *options = nullptr;
		KernelType type;
		
		char * deviceName;
    };

    static AMFKernelStorage * Instance()
    {
        static AMFKernelStorage instance;
        return &instance;
    }

    void PushKernelData(KernelData data)
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
			if (0 == wcscmp(it->kernelid_name, kernelid_name) &&
				0 == strcmp(it->deviceName, deviceName))
			{
				kernelIndex = i;
				break;
			}
		}

		if (kernelIndex < 0)
		{
			if (m_cacheFolder == nullptr)
				return AMF_FAIL;

			char * resultName = ResultFileName(kernelid_name, deviceName, extension);
			FILE *fp = fopen(resultName, "rb");
			if (fp == NULL)
			{
				return AMF_FAIL;
			}
			
			AMFKernelStorage::KernelData kernelData;
			kernelData.type = KernelData::Binary;
			size_t size = wcslen(kernelid_name) + 1;
			kernelData.kernelid_name = (wchar_t *)malloc(size * sizeof(wchar_t));
			wcsncpy(kernelData.kernelid_name, kernelid_name, size);

			size = strlen(kernelName) + 1;
			kernelData.kernelName = (char *)malloc(size * sizeof(char));
			strcpy(kernelData.kernelName, kernelName);

			size = strlen(deviceName) + 1;
			kernelData.deviceName = (char *)malloc(size * sizeof(char));
			strcpy(kernelData.deviceName, deviceName);
			
			fseek(fp, 0, SEEK_END);
			kernelData.dataSize = ftell(fp);
			if (kernelData.dataSize > 0)
			{
				rewind(fp);
				kernelData.data = new unsigned char[kernelData.dataSize];
				fread((void*)kernelData.data, 1, kernelData.dataSize, fp);
			}
			fclose(fp);
			free(resultName);

			kernelIndex = m_cachedKernels.size();
			m_cachedKernels.push_back(kernelData);
		}
		*kernelData = &m_cachedKernels[kernelIndex];
		return AMF_OK;
	}

	char * ResultFileName(const wchar_t *name, const char *deviceName, const char * extension)
	{
		size_t size = wcstombs(nullptr, name, 0) + 1;
		char * cName = (char *)malloc(size * sizeof(char));
		cName[size - 1] = '\0';
		wcstombs(cName, name, size);

		size = wcstombs(nullptr, m_cacheFolder, 0) + 1;
		char * cFolder = (char *)malloc(size * sizeof(char));
		cFolder[size - 1] = '\0';
		wcstombs(cFolder, m_cacheFolder, size);

		size_t foldLen = strlen(cFolder);
		size_t nameLen = strlen(cName);
		size_t extLen = strlen(extension);

		size = foldLen + nameLen + extLen + 2;
		char * resultName = (char* )malloc(size * sizeof(char));
		strcpy(resultName, cFolder);
		resultName[foldLen] = '\\';
		strcpy(resultName + foldLen + 1, cName);
		strcpy(resultName + foldLen + nameLen + 1, extension);
		resultName[size - 1] = '\0';
		return resultName;
	}

	AMF_RESULT SaveProgramBinary(const wchar_t *name, 
		const char *kernelName,
		const char *deviceName,
		const char * extension,
		unsigned char *data, size_t dataSize)
	{
		char * resultName = ResultFileName(name, deviceName, extension);
		FILE *fp = fopen(resultName, "wb");
		if (!fp)
			return AMF_FAIL;
		fwrite(data, 1,	dataSize, fp);
		fclose(fp);
		free(resultName);

		amf_int64 kernelIndex = -1;
		AMF_KERNEL_ID i = 0;
		std::vector<KernelData>::iterator it;
		for (it = m_cachedKernels.begin(); it != m_cachedKernels.end(); it++, i++) {
			if (0 == wcscmp(it->kernelid_name, name) &&
				0 == strcmp(it->deviceName, deviceName))
			{
				kernelIndex = i;
				break;
			}
		}
		if (kernelIndex < 0)
		{
			AMFKernelStorage::KernelData kernelData;
			kernelData.type = KernelData::Binary;
			size_t size = wcslen(name) + 1;
			kernelData.kernelid_name = (wchar_t *)malloc(size * sizeof(wchar_t));
			wcsncpy(kernelData.kernelid_name, name, size);

			size = strlen(kernelName) + 1;
			kernelData.kernelName = (char *)malloc(size * sizeof(char));
			strcpy(kernelData.kernelName, kernelName);

			size = strlen(deviceName) + 1;
			kernelData.deviceName = (char *)malloc(size * sizeof(char));
			strcpy(kernelData.deviceName, deviceName);

			kernelData.dataSize = dataSize;
			kernelData.data = new amf_uint8[dataSize];
			memcpy((void*)kernelData.data, data, dataSize);

			m_cachedKernels.push_back(kernelData);
		}
		else
		{
			m_cachedKernels[kernelIndex].dataSize = dataSize;
			if (m_cachedKernels[kernelIndex].data)
				delete m_cachedKernels[kernelIndex].data;

			m_cachedKernels[kernelIndex].data = new amf_uint8[dataSize];
			memcpy((void*)m_cachedKernels[kernelIndex].data, data, dataSize);
		}

		return AMF_OK;
	}

	AMF_RESULT SetCacheFolder(const wchar_t *path)
	{
		if (m_cacheFolder)
			free(m_cacheFolder);

		size_t size = wcslen(path) + 1;
		m_cacheFolder = (wchar_t *) malloc(size * sizeof(wchar_t));
		wcsncpy(m_cacheFolder, path, size);
		return AMF_OK;
	}

	wchar_t * GetCacheFolder()
	{
		return m_cacheFolder;
	}

	amf_int64 FindSourceIndex(const wchar_t *kernelid_name, const char *options, KernelData::KernelType type = KernelData::Source)
	{
		amf_int64 i = 0;
		std::vector<KernelData>::iterator it;
		for (it = m_kernels.begin(); it != m_kernels.end(); it++, i++) {
			if (it->type != type)
				continue;
			if (0 == wcscmp(it->kernelid_name, kernelid_name))
			{
				if (options == NULL && it->options == NULL)
					return i;
				if (0 == strcmp(it->options, options))
					return i;
			}
		}

		return -1;
	}
	
private:
	AMFKernelStorage() { m_cacheFolder = NULL; }
	~AMFKernelStorage()
	{
		if (m_cacheFolder)
			free(m_cacheFolder);
		//TODO: cleanup kernels
	}

	wchar_t * m_cacheFolder;
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
