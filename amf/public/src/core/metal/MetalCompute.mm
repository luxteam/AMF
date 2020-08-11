#include "MetalCompute.h"

MetalCompute::MetalCompute(id<MTLDevice> device, id<MTLCommandQueue> commandQueue)
 :  m_device(device),
    m_commandQueue(commandQueue)
{
    m_commandBuffer = [m_commandQueue commandBuffer];
    assert(m_commandBuffer != nil);
    //m_library = [m_device newDefaultLibrary];
}

AMF_RESULT MetalCompute::GetKernel(NSString * source, NSString * name, MetalComputeKernel ** kernel)
{
    NSError* error = nil;

    m_library = [m_device newLibraryWithSource: source options:nil error:&error];
    if (m_library == nil)
    {
        NSLog(@"Failed to createLibrary from source.");
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
    id<MTLComputeCommandEncoder> computeEncoder = [m_commandBuffer computeCommandEncoder];
    if (computeEncoder == nil)
    {
        NSLog(@"Failed to created computeEncoder");
        return AMF_FAIL;
    }

    (*kernel) = new MetalComputeKernel(computeEncoder, processFunction, processFunctionPSO);
    return AMF_OK;
}

AMF_RESULT MetalCompute::FlushQueue()
{
    [m_commandBuffer commit];
    return AMF_OK;
}

AMF_RESULT MetalCompute::FinishQueue()
{
    [m_commandBuffer waitUntilCompleted];
    return AMF_OK;
}