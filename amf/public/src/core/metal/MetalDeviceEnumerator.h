#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

class MetalDeviceEnumerator
{
public:
    ~MetalDeviceEnumerator();
    void init();
    int GetDeviceCount() const;

    NSString * GetDeviceName(int index) const;
    id <MTLDevice> GetDeviceAt(int index) const;
private:
    NSArray * m_devices;
};