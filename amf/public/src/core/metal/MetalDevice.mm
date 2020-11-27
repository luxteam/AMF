#include "MetalDevice.h"
#import <Metal/Metal.h>

MetalDevice::MetalDevice(id<MTLDevice> device)
: m_device(device)
{
    m_pageSize = getpagesize();
    m_defaultCommandQueue = [m_device newCommandQueue];
}

MetalDevice::MetalDevice()
{
    // @autoreleasepool{
    //     auto devices = [MTLCopyAllDevices() autorelease];
    //     m_device = [devices[0] retain];
    // }
    m_device = MTLCreateSystemDefaultDevice();
    m_pageSize = getpagesize();
    m_defaultCommandQueue = [m_device newCommandQueue];
}

int buffersCount = 0;

id<MTLBuffer> MetalDevice::AllocateBuffer(size_t size)
{
    //NSLog(@"AllocateBuffer: %d, size: %zu", buffersCount++, size);

    id<MTLBuffer> buffer = [m_device newBufferWithLength:size options:MTLResourceStorageModeShared];
    if(buffer == nil)
    {
        NSLog(@"Failed to create buffer.");

        return nil;
    }

    assert(!(NSUInteger(buffer.contents) % m_pageSize));

    //NSLog(@"Buffer created: %llx", buffer.contents);

    return buffer;
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
        //NSLog(@"CopyBufferToHost finished");
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
    @autoreleasepool
    {
        id<MTLBuffer> tmpBuffer = [m_device newBufferWithLength:size options:MTLResourceStorageModeShared];
        memcpy(tmpBuffer.contents, pSource, size);

        id<MTLCommandBuffer> commandBuffer = [m_defaultCommandQueue commandBuffer];
        if (commandBuffer == nil)
        {
            NSLog(@"Failed to create command buffer.");

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
        [commandBuffer addCompletedHandler:
            ^(id<MTLCommandBuffer> cb)
            {
                //NSLog(@"CopyBufferFromHost finished");
                //TODO: cleanup
            }
            ];

        [commandBuffer commit];
        if (blocking)
        {
            [commandBuffer waitUntilCompleted];
        }
    }

    return AMF_OK;
}

AMF_RESULT MetalDevice::FillBuffer(id<MTLBuffer> pDestHandle, amf_size dstOffset, amf_size dstSize, const void *pSourcePattern, amf_size patternSize)
{
    NSUInteger alignedSize = AlignedValue(patternSize, false);
    id<MTLBuffer> tmpBuffer = [m_device newBufferWithLength:alignedSize options:MTLResourceStorageModeShared];
    memcpy(tmpBuffer.contents, pSourcePattern, patternSize);

    id<MTLCommandBuffer> commandBuffer = [m_defaultCommandQueue commandBuffer];
    if (commandBuffer == nil)
    {
        NSLog(@"Failed to find the process function.");
        return AMF_FAIL;
    }

    id <MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];

    for (int i = 0; i < dstSize; i+=patternSize)
    {
        [blitCommandEncoder
            copyFromBuffer: tmpBuffer
            sourceOffset: 0
            toBuffer: pDestHandle
            destinationOffset: dstOffset + i
            size: patternSize
        ];
    }

    [blitCommandEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];

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

int subBuffersCount = 0;

AMF_RESULT MetalDevice::CreateSubBuffer(id<MTLBuffer> pSourceHandle, void ** subBuffer, amf_size offset, amf_size size)
{
    NSUInteger alignedOffset = AlignedValue(offset, true);
    NSUInteger alignedSize = AlignedValue(size, false);

    if (alignedOffset != offset)
    {
        //NSLog(@"CreateSubBuffer: aligned offset != offset");
        NSLog(@"CreateSubBuffer: aligned offset != offset (%lu - %lu) pageSize = %lu", alignedOffset, offset, m_pageSize);

        return AMF_FAIL;
    }

    unsigned char * dataPtr = static_cast<unsigned char *>(pSourceHandle.contents);

    auto verify = NSUInteger(dataPtr) % m_pageSize;
    if(verify != 0)
    {
        //NSLog(@"CreateSubBuffer: aligned offset != offset");
        NSLog(@"CreateSubBuffer: address are not alligned: %lu", verify);

        return AMF_FAIL;
    }

    //NSLog(
    //    @"CreateSubBuffer: %d, offset: %zu, size: %zu, alignedSize: %lu, source: %llx, ptr: %llx",
    //    subBuffersCount++,
    //    offset,
    //    size,
    //    alignedSize,
    //    pSourceHandle.contents,
    //    (unsigned long)(dataPtr + alignedOffset)
    //    );

    id<MTLBuffer> result = [m_device newBufferWithBytesNoCopy:(dataPtr + alignedOffset)
                                            length: alignedSize
                                            options: MTLResourceStorageModeShared
                                            deallocator: nil];
    if (!result)
    {
        NSLog(@"CreateSubBuffer: result = nil");

        return  AMF_FAIL;
    }

    if ( [result length] != alignedSize)
    {
        NSLog(@"CreateSubBuffer: result length != alignedSize (%lu - %lu) pageSize = %lu", [result length], alignedSize, m_pageSize);

        return  AMF_FAIL;
    }

    (*subBuffer) = (void*)result;

    return AMF_OK;
}

id<MTLCommandQueue> MetalDevice::GetNativeCommandQueue()
{
    return m_defaultCommandQueue;
}

id<MTLDevice> MetalDevice::GetNativeDevice()
{
    return m_device;
}

NSUInteger MetalDevice::AlignedValue(NSUInteger value, bool toLower)
{
    NSUInteger result = (value / m_pageSize) * m_pageSize;
    if (!toLower && result != value)
        result += m_pageSize;
    return result;
}
