#include "MetalCompute.h"
#include "public/common/TraceAdapter.h"

#define AMF_FACILITY L"MetalCompute"

MetalCompute::MetalCompute(id<MTLDevice> device, id<MTLCommandQueue> commandQueue):
    m_device(device),
    m_commandQueue(commandQueue)
{
}

MetalCompute::~MetalCompute()
{
}

AMF_RESULT MetalCompute::GetKernel(NSString * source, NSString * name, MetalComputeKernel ** kernel)
{
    NSError* error = nil;
    MTLCompileOptions * options = [MTLCompileOptions new];

    if (@available(macOS 10.15, iOS 13.0, *))
        options.languageVersion = MTLLanguageVersion::MTLLanguageVersion2_2;

    m_library = [m_device newLibraryWithSource: source options:options error:&error];
    if (m_library == nil)
    {
        NSLog(@"Failed to createLibrary from source: %@ %@", error, [error userInfo]);

        return AMF_FAIL;
    }
    id<MTLFunction> processFunction = [m_library newFunctionWithName:name];
    if (processFunction == nil)
    {
        NSLog(@"Failed to find the process function.");
        return AMF_FAIL;
    }

    id<MTLComputePipelineState> processFunctionPSO = [m_device newComputePipelineStateWithFunction: processFunction error:&error];
    if (processFunctionPSO == nil)
    {
        //  If the Metal API validation is enabled, you can find out more information about what
        //  went wrong.  (Metal API validation is enabled by default when a debug build is run
        //  from Xcode)
        NSLog(@"Failed to created pipeline state object, error %@.", error);
        return AMF_FAIL;
    }

    std::unique_ptr<MetalComputeKernel> kernelPtr(
        new MetalComputeKernel(m_commandQueue, processFunction, processFunctionPSO)
        );
    AMF_RETURN_IF_FALSE(kernelPtr != nullptr, AMF_FAIL);

    (*kernel) = kernelPtr.get();
    m_kernelBuffers.push_back(std::move(kernelPtr));

    return AMF_OK;
}

AMF_RESULT MetalCompute::FlushQueue()
{
    std::for_each(
        m_kernelBuffers.begin(),
        m_kernelBuffers.end(),
        [](std::unique_ptr<MetalComputeKernel> & buffer)
        {
            buffer->FlushQueue();
        }
        );

    return AMF_OK;
}

AMF_RESULT MetalCompute::FinishQueue()
{
    std::for_each(
        m_kernelBuffers.begin(),
        m_kernelBuffers.end(),
        [](std::unique_ptr<MetalComputeKernel> & buffer)
        {
            buffer->FinishQueue();
        }
        );

    return AMF_OK;
}