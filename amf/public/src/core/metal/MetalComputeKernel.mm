#include "MetalComputeKernel.h"

#include "public/common/TraceAdapter.h"

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
    NSLog(@"~MetalComputeKernel");
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
    assert(mPipelineState == PipelineState_New);
    mPipelineState = PipelineState_Enqueued;

    [m_encoder dispatchThreads:workgroupSize
            threadsPerThreadgroup:sizeInWorkgroup];
    [m_encoder endEncoding];

    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::FlushQueue()
{
    assert(mPipelineState == PipelineState_Enqueued);
    mPipelineState = PipelineState_Commited;

    [mCommandBuffer commit];

    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::FinishQueue()
{
    assert(mPipelineState == PipelineState_Commited);
    mPipelineState = PipelineState_Finished;

    [mCommandBuffer waitUntilCompleted];

    return Reset();
}

AMF_RESULT MetalComputeKernel::Reset()
{
    assert(mPipelineState == PipelineState_Finished || mPipelineState == PipelineState_NotSet);

    mCommandBuffer = [mCommandQueue commandBuffer];

    m_encoder = [mCommandBuffer computeCommandEncoder];
    assert(m_encoder != nil);

    [m_encoder setComputePipelineState:m_processFunctionPSO];

    mPipelineState = PipelineState_New;

    return mCommandBuffer && m_encoder
        ? AMF_OK
        : AMF_FAIL;
}

