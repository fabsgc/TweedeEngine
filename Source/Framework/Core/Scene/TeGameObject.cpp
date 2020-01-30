#include "Scene/TeGameObject.h"
#include "Scene/TeGameObjectManager.h"

namespace te
{
    void GameObject::Initialize(const SPtr<GameObject>& object, UINT64 instanceId)
    {
        _instanceData = te_shared_ptr_new<GameObjectInstanceData>();
        _instanceData->Object = object;
        _instanceData->InstanceId = instanceId;
    }

    void GameObject::_setInstanceData(GameObjectInstanceDataPtr& other)
    {
        SPtr<GameObject> myPtr = _instanceData->Object;
        UINT64 oldId = _instanceData->InstanceId;

        _instanceData = other;
        _instanceData->Object = myPtr;

        GameObjectManager::Instance().RemapId(oldId, _instanceData->InstanceId);
    }
}
