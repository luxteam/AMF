#pragma once
#include "../../../include/core/Result.h"
#include "MetalComputeWrapper.h"

class MetalDevice;

class MetalDeviceWrapper
{
public:

    MetalDeviceWrapper(void * native);

    void * AllocateBuffer(size_t size);
    AMF_RESULT ReleaseBuffer(void * native);

    AMF_RESULT CopyBuffer(void * pDestHandle, size_t dstOffset, void * pSourceHandle, size_t srcOffset, size_t size);
    AMF_RESULT CopyBufferToHost(void *pDest, void * pSourceHandle, size_t srcOffset, size_t size, bool blocking);
    AMF_RESULT CopyBufferFromHost(void * pDestHandle, size_t dstOffset, const void *pSource, size_t size, bool blocking);

    AMF_RESULT CreateCompute(MetalComputeWrapper ** compute);

    void * GetNativeDevice();
private:
    MetalDevice * m_device;
};