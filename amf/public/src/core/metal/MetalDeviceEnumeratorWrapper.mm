#include "MetalDeviceEnumeratorWrapper.h"
#include "MetalDeviceEnumerator.h"

MetalDeviceEnumeratorWrapper::MetalDeviceEnumeratorWrapper()
{
    m_device = new MetalDeviceEnumerator();
}

MetalDeviceEnumeratorWrapper::~MetalDeviceEnumeratorWrapper()
{
    delete m_device;
}

void MetalDeviceEnumeratorWrapper::init()
{
    m_device->init();
}
int MetalDeviceEnumeratorWrapper::GetDeviceCount() const
{
    return m_device->GetDeviceCount();
}
char * MetalDeviceEnumeratorWrapper::GetDeviceName(int index) const
{
    NSString * result = m_device->GetDeviceName(index);
    return (char*) [result UTF8String];
}
void * MetalDeviceEnumeratorWrapper::GetDeviceAt(int index) const
{
    return m_device->GetDeviceAt(index);
}