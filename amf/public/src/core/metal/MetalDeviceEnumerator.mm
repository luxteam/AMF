#include "MetalDeviceEnumerator.h"


MetalDeviceEnumerator::~MetalDeviceEnumerator()
{
    [m_devices release];
}

void MetalDeviceEnumerator::init()
{
    m_devices = MTLCopyAllDevices();
}

int MetalDeviceEnumerator::GetDeviceCount() const
{
    return [m_devices count];
}

NSString * MetalDeviceEnumerator::GetDeviceName(int index) const
{
    return [GetDeviceAt(index) name];
}

id <MTLDevice> MetalDeviceEnumerator::GetDeviceAt(int index) const
{
    return [m_devices objectAtIndex: index];
}