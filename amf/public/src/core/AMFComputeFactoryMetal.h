#include "../../include/core/Compute.h"
#include "../../include/core/ComputeFactory.h"
#include "../../common/InterfaceImpl.h"
#include "../../common/AMFSTL.h"
#include "../../common/TraceAdapter.h"
#include "../../common/PropertyStorageImpl.h"
#include "AMFDeviceMetalImpl.h"
#include "metal/MetalDeviceEnumeratorWrapper.h"

#include <memory>

class AMFComputeFactoryMetal : public AMFInterfaceImpl<AMFComputeFactory>
{
public:
    AMFComputeFactoryMetal(AMFContextImpl* pContext);
	~AMFComputeFactoryMetal();
    AMF_RESULT Init();
    char * GetDeviceName(int index) const;
    virtual amf_int32           AMF_STD_CALL GetDeviceCount() override;
    virtual AMF_RESULT          AMF_STD_CALL GetDeviceAt(amf_int32 index, AMFComputeDevice **ppDevice) override;
private:
    amf_vector<AMFDevicePtr> m_devices;
    AMFContextImpl* m_pContext;
    std::unique_ptr<MetalDeviceEnumeratorWrapper> m_wrapper;
};