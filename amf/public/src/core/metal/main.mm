#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include "MetalDevice.h"

// The number of floats in each array, and the size of the arrays in bytes.
const unsigned int arrayLength = 1 << 12;
const unsigned int bufferSize = arrayLength * sizeof(float);


int main(int argc, const char * argv[]) {
    @autoreleasepool {
        MetalDevice device (MTLCreateSystemDefaultDevice());

        id<MTLBuffer> input = device.AllocateBuffer(bufferSize);
        id<MTLBuffer> output = device.AllocateBuffer(bufferSize);

        AMF_RESULT res = AMF_OK;

        float data [arrayLength];
        float result [arrayLength];

        for (unsigned long index = 0; index < arrayLength; index++)
        {
            data[index] = (float)rand()/(float)(RAND_MAX);
            result[index] = 0;
        }

        res = device.CopyBufferFromHost(input, 0, &data[0], bufferSize, true);
        MetalCompute * compute;
        res = device.CreateCompute(&compute);

        MetalComputeKernel * kernel;
        res = compute->GetKernel(@"process_array", &kernel);

        res = kernel->SetArgBuffer(input, 0);
        res = kernel->SetArgBuffer(output, 1);

        MTLSize gridSize = MTLSizeMake(arrayLength, 1, 1);
        MTLSize threadgroupSize = kernel->GetCompileWorkgroupSize(arrayLength);

        res = kernel->Enqueue(gridSize, threadgroupSize);

        res = compute->FlushQueue();
        res = compute->FinishQueue();

        res = device.CopyBufferToHost(&result[0], output, 0, bufferSize, true);

        for (unsigned long index = 0; index < 10; index++)
        {
            NSString * str = [NSString stringWithFormat: @"%f", result[index]];
            NSLog(str);
        }
    }
    return 0;
}
