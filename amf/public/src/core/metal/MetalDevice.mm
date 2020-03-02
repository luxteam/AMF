#include "MetalDevice.h"

MetalDevice::MetalDevice(id<MTLDevice> device)
: m_device(device)
{
    if (!m_device)
    {
        m_device = MTLCreateSystemDefaultDevice();
    }
    m_defaultCommandQueue = [m_device newCommandQueue];
}

id<MTLBuffer> MetalDevice::AllocateBuffer(size_t size)
{
    return [m_device newBufferWithLength:size options:MTLResourceStorageModePrivate];
}

AMF_RESULT MetalDevice::ReleaseBuffer(id<MTLBuffer>  buffer)
{
    [buffer release];
    return AMF_OK;
}

AMF_RESULT MetalDevice::CopyBuffer(id<MTLBuffer> pDestHandle, size_t dstOffset, id<MTLBuffer> pSourceHandle, size_t srcOffset, size_t size)
{
    @autoreleasepool {
        id<MTLCommandBuffer> commandBuffer = [m_defaultCommandQueue commandBuffer];
        if (commandBuffer == nil)
        {
            NSLog(@"Failed to find the process function.");
            return AMF_FAIL;
        }

        id <MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
        [blitCommandEncoder
            copyFromBuffer: pSourceHandle
            sourceOffset: srcOffset
            toBuffer: pDestHandle
            destinationOffset: dstOffset
            size: size
        ];

        [blitCommandEncoder endEncoding];
        [commandBuffer commit];
        [commandBuffer waitUntilCompleted];
    }
    return AMF_OK;
}

AMF_RESULT MetalDevice::CopyBufferToHost(void *pDest, id<MTLBuffer> pSourceHandle, size_t srcOffset, size_t size, bool blocking)
{
    id<MTLBuffer> tmpBuffer = [m_device newBufferWithLength:size options:MTLResourceStorageModeShared];

    id<MTLCommandBuffer> commandBuffer = [m_defaultCommandQueue commandBuffer];
    if (commandBuffer == nil)
    {
        NSLog(@"Failed to find the process function.");
        return AMF_FAIL;
    }

    id <MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
    [blitCommandEncoder
        copyFromBuffer: pSourceHandle
        sourceOffset: srcOffset
        toBuffer: tmpBuffer
        destinationOffset: 0
        size: size
    ];

    [blitCommandEncoder endEncoding];
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
        NSLog(@"CopyBufferToHost finished");
        memcpy(pDest, tmpBuffer.contents, size);
        //TODO: cleanup
    }];

    [commandBuffer commit];
    if (blocking)
    {
        [commandBuffer waitUntilCompleted];
        memcpy(pDest, tmpBuffer.contents, size);
    }
    return AMF_OK;
}

AMF_RESULT MetalDevice::CopyBufferFromHost(id<MTLBuffer> pDestHandle, size_t dstOffset, const void *pSource, size_t size, bool blocking)
{
    id<MTLBuffer> tmpBuffer = [m_device newBufferWithLength:size options:MTLResourceStorageModeShared];
    memcpy(tmpBuffer.contents, pSource, size);

    id<MTLCommandBuffer> commandBuffer = [m_defaultCommandQueue commandBuffer];
    if (commandBuffer == nil)
    {
        NSLog(@"Failed to find the process function.");
        return AMF_FAIL;
    }

    id <MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
    [blitCommandEncoder
        copyFromBuffer: tmpBuffer
        sourceOffset: 0
        toBuffer: pDestHandle
        destinationOffset: dstOffset
        size: size
    ];

    [blitCommandEncoder endEncoding];
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
        NSLog(@"CopyBufferFromHost finished");
        //TODO: cleanup
    }];

    [commandBuffer commit];
    if (blocking)
    {
        [commandBuffer waitUntilCompleted];
    }
    return AMF_OK;
}

AMF_RESULT MetalDevice::CreateCompute(MetalCompute ** compute)
{
    id<MTLCommandQueue> commandQueue = [m_device newCommandQueue];
    if (commandQueue == nil)
    {
        NSLog(@"Failed to find the command queue.");
        return AMF_FAIL;
    }
    (*compute) = new MetalCompute(m_device, commandQueue);
    return AMF_OK;
}

id<MTLDevice> MetalDevice::GetNativeDevice()
{
    return m_device;
}