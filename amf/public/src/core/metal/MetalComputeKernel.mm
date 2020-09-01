#include "MetalComputeKernel.h"

MetalComputeKernel::MetalComputeKernel( id<MTLCommandBuffer> buffer,
                        id<MTLFunction> processFunction,
                        id<MTLComputePipelineState> processFunctionPSO)
    :   m_buffer(buffer),
        m_processFunction(processFunction),
        m_processFunctionPSO(processFunctionPSO)
{
    m_encoder = [m_buffer computeCommandEncoder];
    assert(m_encoder != nil);
    [m_encoder setComputePipelineState:m_processFunctionPSO];
}

AMF_RESULT MetalComputeKernel::SetArgBuffer(id<MTLBuffer> buffer, int index)
{
    [m_encoder setBuffer:buffer offset:0 atIndex:index];
    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::SetArgInt32(int32_t value, int index)
{
    [m_encoder setBytes:&value length:sizeof(int32_t) atIndex:index];
    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::SetArgInt64(int64_t value, int index)
{
    [m_encoder setBytes:&value length:sizeof(int64_t) atIndex:index];
    return AMF_OK;
}

AMF_RESULT MetalComputeKernel::SetArgFloat(float value, int index)
{
    [m_encoder setBytes:&value length:sizeof(float) atIndex:index];
    return AMF_OK;
}

MTLSize MetalComputeKernel::GetCompileWorkgroupSize(NSUInteger maxSize)
{
    NSUInteger threadGroupSize = m_processFunctionPSO.maxTotalThreadsPerThreadgroup;
    if (threadGroupSize > maxSize)
        return MTLSizeMake(maxSize, 1, 1);

    return MTLSizeMake(threadGroupSize, 1, 1);
}

AMF_RESULT MetalComputeKernel::Enqueue(MTLSize workgroupSize, MTLSize sizeInWorkgroup)
{
    [m_encoder dispatchThreads:workgroupSize
            threadsPerThreadgroup:sizeInWorkgroup];
    [m_encoder endEncoding];
    return AMF_OK;
}
