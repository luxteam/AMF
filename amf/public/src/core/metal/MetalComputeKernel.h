#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include "../../../include/core/Result.h"

class MetalComputeKernel
{
public:
    MetalComputeKernel(
        id<MTLCommandQueue>         queue,
        id<MTLFunction>             processFunction,
        id<MTLComputePipelineState> processFunctionPSO
        );

    AMF_RESULT SetArgBuffer(id<MTLBuffer> buffer, int index);
    AMF_RESULT SetArgInt32(int32_t value, int index);
    AMF_RESULT SetArgInt64(int64_t value, int index);
    AMF_RESULT SetArgFloat(float value, int index);

    MTLSize GetCompileWorkgroupSize(MTLSize maxSize);

    AMF_RESULT Enqueue(MTLSize workgroupSize, MTLSize sizeInWorkgroup);

    virtual AMF_RESULT Reset();
    virtual AMF_RESULT FlushQueue();
    virtual AMF_RESULT FinishQueue();

private:
    id<MTLCommandQueue> mCommandQueue;
    id<MTLCommandBuffer> mCommandBuffer;
    id<MTLComputeCommandEncoder> m_encoder;
    id<MTLFunction> m_processFunction;
    id<MTLComputePipelineState> m_processFunctionPSO;
};