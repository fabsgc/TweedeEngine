#include "Scene/TeComponent.h"
#include "Scene/TeSceneObject.h"

namespace te
{ 
    Component::Component(HSceneObject parent, UINT32 type)
        : Serializable(type)
        , _parent(std::move(parent))
    {
        SetName("Component");
    }

    bool Component::TypeEquals(const Component& other)
    {
        return GetCoreType() == other.GetCoreType();
    }

    bool Component::CalculateBounds(Bounds& bounds)
    {
        Vector3 position = SO()->GetTransform().GetPosition();

        bounds = Bounds(AABox(position, position), Sphere(position, 0.0f));
        return false;
    }

    void Component::Destroy(bool immediate)
    {
        SO()->DestroyComponent(this, immediate);
    }

    void Component::DestroyInternal(GameObjectHandleBase& handle, bool immediate)
    {
        if (immediate)
            GameObjectManager::Instance().UnregisterObject(handle);
        else
            GameObjectManager::Instance().QueueForDestroy(handle);
    }
}
