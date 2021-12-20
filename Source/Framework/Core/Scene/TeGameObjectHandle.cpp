#include "TeCorePrerequisites.h"
#include "Scene/TeGameObjectHandle.h"
#include "Scene/TeGameObject.h"

namespace te
{ 
    GameObjectHandleBase::GameObjectHandleBase(const SPtr<GameObject>& ptr)
    {
        _data = te_shared_ptr_new<GameObjectHandleData>(ptr->_instanceData);
    }

    bool GameObjectHandleBase::IsDestroyed(bool checkQueued) const
    {
        return _data->Ptr == nullptr || _data->Ptr->Object == nullptr
            || (checkQueued && _data->Ptr->Object->GetIsDestroyed());
    }

    void GameObjectHandleBase::SetHandleData(const SPtr<GameObject>& object)
    {
        _data->Ptr = object->_instanceData;
    }

    void GameObjectHandleBase::ThrowIfDestroyed() const
    {
        if (IsDestroyed())
        {
            TE_ASSERT_ERROR(false, "Trying to access an object that has been destroyed.");
        }
    }
}
