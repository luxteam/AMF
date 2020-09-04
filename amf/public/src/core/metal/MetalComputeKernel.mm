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

MTLSize MetalComputeKernel::GetCompileWorkgroupSize(MTLSize maxSize)
{
    NSUInteger w = m_processFunctionPSO.threadExecutionWidth;
    if (w > maxSize.width)
        w = maxSize.width;
    NSUInteger h = m_processFunctionPSO.maxTotalThreadsPerThreadgroup / w;
    if (h > maxSize.height)
        h = maxSize.height;
    
    int d = m_processFunctionPSO.maxTotalThreadsPerThreadgroup / (w * h);
    return MTLSizeMake(w, h, 1);
}

AMF_RESULT MetalComputeKernel::Enqueue(MTLSize workgroupSize, MTLSize sizeInWorkgroup)
{
    [m_encoder dispatchThreads:workgroupSize
            threadsPerThreadgroup:sizeInWorkgroup];
    [m_encoder endEncoding];
    return AMF_OK;
}
