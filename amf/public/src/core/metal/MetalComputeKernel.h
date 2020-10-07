#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include "../../../include/core/Result.h"
#include <vector>

class MetalComputeKernel
{
public:
    MetalComputeKernel(
        id<MTLCommandQueue>         queue,
        id<MTLFunction>             processFunction,
        id<MTLComputePipelineState> processFunctionPSO
        );
    
    struct Bindind
    {
        enum Type{
            Buffer,
            Int64,
            Int32,
            Float
        };
        Type type;
        int index;
        union {
            int64_t value64;
            int32_t value32;
            float valueFloat;
            id<MTLBuffer> buffer;
        };
    };
    ~MetalComputeKernel();

    AMF_RESULT SetArgBuffer(id<MTLBuffer> buffer, int index);
    AMF_RESULT SetArgInt32(int32_t value, int index);
    AMF_RESULT SetArgInt64(int64_t value, int index);
    AMF_RESULT SetArgFloat(float value, int index);

    MTLSize GetCompileWorkgroupSize(MTLSize maxSize);

    AMF_RESULT Enqueue(MTLSize workgroupSize, MTLSize sizeInWorkgroup);

    virtual AMF_RESULT Reset();
    virtual AMF_RESULT FlushQueue();
    virtual AMF_RESULT FinishQueue();

private:
    enum PipelineState
    {
        PipelineState_NotSet,
        PipelineState_New,
        PipelineState_Enqueued,
        PipelineState_Commited,
        PipelineState_Finished
    };

    //make a new current global pool (the "new pool")
    //NSAutoreleasePool * newPool = [[NSAutoreleasePool alloc] init];
    NSAutoreleasePool * mLocalPool = nullptr;

    PipelineState mPipelineState = PipelineState_NotSet;

    id<MTLCommandQueue> mCommandQueue;
    id<MTLFunction> m_processFunction;
    id<MTLComputePipelineState> m_processFunctionPSO;
    std::vector<MetalComputeKernel::Bindind> m_bindings;
    MTLSize m_workgroupSize;
    MTLSize m_sizeInWorkgroup;
};
