#pragma once
#include "../../../include/core/Result.h"

class MetalDeviceEnumerator;

class MetalDeviceEnumeratorWrapper
{
public:

    MetalDeviceEnumeratorWrapper();
    ~MetalDeviceEnumeratorWrapper();

    void init();
    int GetDeviceCount() const;
    char * GetDeviceName(int index) const;
    void * GetDeviceAt(int index) const;
private:
    MetalDeviceEnumerator * m_device;
};