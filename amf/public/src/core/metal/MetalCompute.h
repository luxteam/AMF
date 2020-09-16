#import <Metal/Metal.h>
#include "MetalComputeKernel.h"
#include "../../../include/core/Result.h"

#include <vector>
#include <memory>

class MetalCompute
{
public:
    MetalCompute(id<MTLDevice> device, id<MTLCommandQueue> commandQueue);
    ~MetalCompute();
    AMF_RESULT GetKernel(NSString * source, NSString * name, MetalComputeKernel ** kernel);

    AMF_RESULT FlushQueue();
    AMF_RESULT FinishQueue();

private:
    id<MTLDevice> m_device;
    id<MTLCommandQueue> m_commandQueue;
    id<MTLLibrary> m_library;

    std::vector<std::unique_ptr<MetalComputeKernel>> m_kernelBuffers;
};
