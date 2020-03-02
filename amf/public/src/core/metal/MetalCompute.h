#import <Metal/Metal.h>
#include "MetalComputeKernel.h"
#include "../../../include/core/Result.h"

class MetalCompute
{
public:
    MetalCompute(id<MTLDevice> device, id<MTLCommandQueue> commandQueue);

    AMF_RESULT GetKernel(NSString * name, MetalComputeKernel ** kernel);

    AMF_RESULT FlushQueue();
    AMF_RESULT FinishQueue();

private:
    id<MTLCommandQueue> m_commandQueue;
    id<MTLDevice> m_device;
    id<MTLLibrary> m_library;
    id<MTLCommandBuffer> m_commandBuffer;
};