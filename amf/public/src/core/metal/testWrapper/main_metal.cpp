#include "../AMFDeviceMetal.h"
#include "../../ProgramsImpl.h"

#define CL_TARGET_OPENCL_VERSION 120

int main(int argc, char *argv[])
{
    AMFDeviceMetalImpl * device = new AMFDeviceMetalImpl(NULL, NULL);
    AMFProgramsImpl pPrograms;

    amf::AMF_KERNEL_ID kernel = 0;
    const char* kernel_src = "process_array";
    pPrograms.RegisterKernelSource(&kernel, L"kernelIDName", "process_array", strlen(kernel_src), (amf_uint8*)kernel_src, "option");

	AMF_RESULT res;

    //amf::AMFComputeDevicePtr pComputeDevice;

    amf::AMFComputePtr pCompute;
    device->CreateCompute(nullptr, &pCompute);

    amf::AMFComputeKernelPtr pKernel;
    res = pCompute->GetKernel(kernel, &pKernel);

    return 0;
}
