#ifndef DATAIMPL_H
#define DATAIMPL_H

#include "../../include/core/Data.h"
#include "../../common/InterfaceImpl.h"
#include "../../common/PropertyStorageImpl.h"
#include "ContextImpl.h"

using namespace amf;

template<typename _TBase>
class AMFDataImpl :
        public AMFPropertyStorageImpl<_TBase>
{
public:
    AMFDataImpl(AMFContextImpl *pContext)
        : m_pts(-1LL),
        m_duration(-1LL),
        m_pContext(pContext)
    {
        m_pContext->Acquire();
    }
    virtual ~AMFDataImpl()
    {
        m_pContext = nullptr;
    }
    // interface access
    AMF_BEGIN_INTERFACE_MAP
    AMF_INTERFACE_ENTRY(AMFData)
    AMF_INTERFACE_CHAIN_ENTRY(AMFPropertyStorageImpl<_TBase>)
    AMF_END_INTERFACE_MAP

    // AMFData interface
public:

	virtual amf_bool AMF_STD_CALL  IsReusable()
    {
        return true;
    }

    virtual void AMF_STD_CALL  SetPts(amf_pts pts)
    {
        m_pts = pts;
    }

    virtual amf_pts AMF_STD_CALL GetPts()
    {
        return m_pts;
    }

    virtual void AMF_STD_CALL  SetDuration(amf_pts duration)
    {
        m_duration = duration;
    }

    virtual amf_pts AMF_STD_CALL GetDuration()
    {
        return m_duration;
    }
protected:
    AMFContextImpl* GetContext() { return m_pContext; }
private:
    amf_pts m_pts;
    amf_pts m_duration;
    AMFContextImpl* m_pContext;

    AMFDataImpl(const AMFDataImpl&);
    AMFDataImpl& operator=(const AMFDataImpl&);
};


#endif // DATAIMPL_H
