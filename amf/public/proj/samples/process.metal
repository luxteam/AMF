#include <metal_stdlib>
using namespace metal;

kernel void process_array(device const float* inA,
                       device float* result,
                       uint index [[thread_position_in_grid]])
{
    result[index] = inA[index] * inA[index] * inA[index] ;
}
