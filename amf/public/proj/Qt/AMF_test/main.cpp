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


    oclComputeFactory;

    return a.exec();
}
