#include <QCoreApplication>
#include "../../../include/core/Factory.h"
#include "../../../common/AMFFactory.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    AMFFactoryHelper helper;
    helper.Init();
    amf::AMFFactory* factory = helper.GetFactory();

    amf::AMFContextPtr context;
    factory->CreateContext(&context);
    amf::AMFComputeFactoryPtr oclComputeFactory;
    context->GetOpenCLComputeFactory(&oclComputeFactory);


    int deviceCount = oclComputeFactory->GetDeviceCount();
    for(int i = 0; i < deviceCount; ++i)
    {
        amf::AMFComputeDevicePtr pComputeDevice;
        oclComputeFactory->GetDeviceAt(i, &pComputeDevice);
        pComputeDevice->GetNativeContext();
        amf::AMFComputePtr pCompute;
        pComputeDevice->CreateCompute(nullptr, &pCompute);

        amf::AMF_KERNEL_ID kernel = 0;//tbd
        amf::AMFComputeKernelPtr pKernel;
        pCompute->GetKernel(kernel, &pKernel);
        //pKernel->SetArgInt32(...);
        //pKernel->SetArgBuffer(...);
        //pKernel->Enqueue(...);
        //pCompute->FlushQueue();
    }

    return a.exec();
}
