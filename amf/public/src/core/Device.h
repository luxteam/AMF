#ifndef AMFDEVICE_H
#define AMFDEVICE_H

#include "../../include/core/Interface.h"
#include "../../include/core/Data.h"
#include "../../include/core/Compute.h"

namespace amf
{
    class AMFDevice : virtual public AMFInterface
    {
    public:
        virtual AMF_MEMORY_TYPE GetType() = 0;

        virtual AMF_RESULT AllocateBuffer(amf_size size, void** ppHandle) = 0;
        virtual AMF_RESULT AttachBuffer(amf_size size, void* pHandle) = 0;
        virtual AMF_RESULT ReleaseBuffer(void* pHandle, bool attached) = 0;
		virtual AMF_RESULT CreateSubBuffer(AMFBuffer* pHandle, void ** subBuffer, amf_size offset, amf_size size) = 0;

        virtual AMF_RESULT CopyBuffer(void* pDestHandle, amf_size dstOffset, void* pSourceHandle, amf_size srcOffset, amf_size size) = 0;
        virtual AMF_RESULT CopyBufferToHost(void* pDest, void* pSourceHandle, amf_size srcOffset, amf_size size, bool blocking) = 0;
        virtual AMF_RESULT CopyBufferFromHost(void* pDestHandle, amf_size dstOffset, const void* pSource, amf_size size, bool blocking) = 0;
        virtual AMF_RESULT FillBuffer(void* pDestHandle, amf_size dstOffset, amf_size dstSize, const void* pSourcePattern, amf_size patternSize) = 0;
    };

    typedef AMFInterfacePtr_T<AMFDevice> AMFDevicePtr;

}

#endif // AMFDEVICE_H
