#include "TeCoreObject.h"

namespace te
{
    CoreObject::CoreObject()
        : _flags(CGO_NONE)
        , _coreDirtyFlags(0)
        , _internalID(CoreObjectManager::Instance().GenerateId())
    {
    }

    CoreObject::~CoreObject()
    {
        if(!IsDestroyed())
        {
            // Object must be released with Destroy() otherwise engine can still try to use it, even if it was destructed
            // (e.g. if an object has one of its methods queued in a command queue, and is destructed, you will be accessing invalid memory)
            TE_ASSERT_ERROR(false, "Destructor called but object is not destroyed. This will result in nasty issues.");
        }

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if(!_this.expired())
        {
            TE_ASSERT_ERROR(false, "Shared pointer to this object still has active references but the object is being deleted? You shouldn't delete CoreObjects manually.");
        }
#endif
    }

    void CoreObject::Initialize()
    {
        CoreObjectManager::Instance().RegisterObject(this);
        _flags |= CGO_INITIALIZED;
    }

    void CoreObject::Destroy()
    {
        CoreObjectManager::Instance().UnregisterObject(this);
        _flags |= CGO_DESTROYED;
    }

    void CoreObject::SetThisPtr(SPtr<CoreObject> ptrThis)
    {
        _this = ptrThis;
    }

    void CoreObject::MarkCoreDirty(UINT32 flags)
    {
        bool wasDirty = IsCoreDirty();

        _coreDirtyFlags |= flags;

        if (!wasDirty && IsCoreDirty())
            CoreObjectManager::Instance().NotifyCoreDirty(this);
    }

    void CoreObject::RemoveDirtyFlag(UINT32 flags)
    {
        _coreDirtyFlags &= ~flags;
    }
}
