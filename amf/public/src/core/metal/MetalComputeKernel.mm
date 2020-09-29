#include "MetalComputeKernel.h"

#include "public/common/TraceAdapter.h"

#include <thread>
#include <iostream>

#define AMF_FACILITY L"MetalComputeKernel"

MetalComputeKernel::MetalComputeKernel(
    id<MTLCommandQueue>         queue,
    id<MTLFunction>             processFunction,
    id<MTLComputePipelineState> processFunctionPSO
    ):
    mCommandQueue(queue),
    m_processFunction(processFunction),
    m_processFunctionPSO(processFunctionPSO)
{
    Reset();
}

MetalComputeKernel::~MetalComputeKernel()
{
    NSLog(@"~MetalComputeKernel %p", this);
}

AMF_RESULT MetalComputeKernel::SetArgBuffer(id<MTLBuffer> buffer, int index)
{
    assert(mPipelineState == PipelineState_New);

    [m_encoder setBuffer:buffer offset:0 atIndex:index];

    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::SetArgInt32(int32_t value, int index)
{
    assert(mPipelineState == PipelineState_New);

    [m_encoder setBytes:&value length:sizeof(int32_t) atIndex:index];

    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::SetArgInt64(int64_t value, int index)
{
    assert(mPipelineState == PipelineState_New);

    [m_encoder setBytes:&value length:sizeof(int64_t) atIndex:index];

    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::SetArgFloat(float value, int index)
{
    assert(mPipelineState == PipelineState_New);

    [m_encoder setBytes:&value length:sizeof(float) atIndex:index];

    return AMF_OK;
}

MTLSize MetalComputeKernel::GetCompileWorkgroupSize(MTLSize maxSize)
{
    NSUInteger w = m_processFunctionPSO.threadExecutionWidth;
    if (w > maxSize.width)
        w = maxSize.width;
    NSUInteger h = m_processFunctionPSO.maxTotalThreadsPerThreadgroup / w;
    if (h > maxSize.height)
        h = maxSize.height;

    //int d = m_processFunctionPSO.maxTotalThreadsPerThreadgroup / (w * h);

    return MTLSizeMake(w, h, 1);
}

AMF_RESULT MetalComputeKernel::Enqueue(MTLSize workgroupSize, MTLSize sizeInWorkgroup)
{
    //std::cout << ">>MCK::Enq " << std::this_thread::get_id() << ": " << this << std::endl;
    //NSLog(@">>MCK::Enq %d %p", std::this_thread::get_id(), this);

    assert(mPipelineState == PipelineState_New);
    mPipelineState = PipelineState_Enqueued;

    NSLog(@">>command buffer %llx status =  %lu.", mCommandBuffer, [mCommandBuffer status]);
    [m_encoder dispatchThreads:workgroupSize
            threadsPerThreadgroup:sizeInWorkgroup];
    [m_encoder endEncoding];
    [mCommandBuffer enqueue];
    NSLog(@"<<command buffer %llx status =  %lu.", mCommandBuffer, [mCommandBuffer status]);

    //NSLog(@"<<MCK::Enq");

    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::FlushQueue()
{
    //std::cout << ">>MCK::Flu " << std::this_thread::get_id() << ": " << this << std::endl;

    assert(mPipelineState == PipelineState_Enqueued);
    mPipelineState = PipelineState_Commited;

    NSLog(@">>command buffer %llx status =  %lu.", mCommandBuffer, [mCommandBuffer status]);
    [mCommandBuffer commit];
    NSLog(@"<<command buffer %llx status =  %lu.", mCommandBuffer, [mCommandBuffer status]);
    
    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::FinishQueue()
{
    //std::cout << ">>MCK::Fin " << std::this_thread::get_id() << ": " << this << std::endl;

    assert(mPipelineState == PipelineState_Commited);
    mPipelineState = PipelineState_Finished;

    NSLog(@">>command buffer %llx status =  %lu.", mCommandBuffer, [mCommandBuffer status]);
    [mCommandBuffer waitUntilCompleted];
    NSLog(@"<<command buffer %llx status =  %lu.", mCommandBuffer, [mCommandBuffer status]);

    return Reset();
}

AMF_RESULT MetalComputeKernel::Reset()
{
    //std::cout << ">>MCK::Res " << std::this_thread::get_id() << ": " << this << std::endl;

    assert(mPipelineState == PipelineState_Finished || mPipelineState == PipelineState_NotSet);

    if(mPipelineState == PipelineState_Finished)
    {
        NSLog(@"used retain %llx %llx is %d %d %d", m_encoder, mCommandBuffer, [m_encoder retainCount], [mCommandBuffer retainCount], [mCommandQueue retainCount]);
        //[m_encoder release];
        //[mCommandQueue release];
        //[m_encoder autorelease];
        //[mCommandQueue autorelease];

        //NSLog(@"Retain count2 is %d %d", [m_encoder retainCount], [mCommandQueue retainCount]);
        //[mCommandBuffer release];

        //[mLocalPool drain];
        //NSLog(@"Retain count3 is %d %d", [m_encoder retainCount], [mCommandQueue retainCount]);
        
        [mLocalPool release];
        //NSLog(@"Retain count2 is %d %d", [m_encoder retainCount], [mCommandQueue retainCount]);
    }

    mLocalPool = [[NSAutoreleasePool alloc] init];
    
    mCommandBuffer = [mCommandQueue commandBuffer];
    //[newPool addObject: mCommandBuffer];
    //mCommandBuffer = [[mCommandQueue commandBuffer] autorelease];

    //m_encoder = [mCommandBuffer computeCommandEncoder];
    m_encoder = [mCommandBuffer computeCommandEncoderWithDispatchType:MTLDispatchTypeConcurrent];
    //m_encoder = [[mCommandBuffer computeCommandEncoderWithDispatchType:MTLDispatchTypeConcurrent] autorelease];
    [m_encoder setComputePipelineState:m_processFunctionPSO];

    NSLog(@"new retain %llx %llx is %d %d %d", m_encoder, mCommandBuffer, [m_encoder retainCount], [mCommandBuffer retainCount], [mCommandQueue retainCount]);

    mPipelineState = PipelineState_New;

    //NSLog(@"<<MCK::Res");

    return mCommandBuffer && m_encoder
        ? AMF_OK
        : AMF_FAIL;
}
