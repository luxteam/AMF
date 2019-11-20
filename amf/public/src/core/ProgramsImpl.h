#ifndef PROGRAMSIMPL_H
#define PROGRAMSIMPL_H

#include "../../include/core/Compute.h"
#include <vector>

using namespace amf;

class AMFKernelStorage
{
public:
    struct KernelData
    {
        AMF_KERNEL_ID *pKernelID;
        const wchar_t *kernelid_name;
        const char *kernelName;
        amf_size dataSize;
        const amf_uint8 *data;
        const char *options;
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
        //TODO: check bounds
        *kernelData = &m_kernels[kernelID];
        return AMF_OK;
    }
private:
    AMFKernelStorage(){}


    std::vector<KernelData> m_kernels;
};


class AMFProgramsImpl : public AMFPrograms
{
public:
    AMFProgramsImpl();

    // AMFPrograms interface
public:
    virtual AMF_RESULT RegisterKernelSourceFile(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, const wchar_t *filepath, const char *options) override;
    virtual AMF_RESULT RegisterKernelSource(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
    virtual AMF_RESULT RegisterKernelBinary(AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
    virtual AMF_RESULT RegisterKernelSource1(AMF_MEMORY_TYPE eMemoryType, AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
    virtual AMF_RESULT RegisterKernelBinary1(AMF_MEMORY_TYPE eMemoryType, AMF_KERNEL_ID *pKernelID, const wchar_t *kernelid_name, const char *kernelName, amf_size dataSize, const amf_uint8 *data, const char *options) override;
};

#endif // PROGRAMSIMPL_H
