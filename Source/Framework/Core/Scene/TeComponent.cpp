#include "Scene/TeComponent.h"
#include "Scene/TeSceneObject.h"
#include "Math/TeBounds.h"

namespace te
{ 
    UINT32 Component::ComponentType = TID_Component;
    const UINT32 Component::AlwaysRun = 1;

    Event<void(const HComponent&)> Component::OnComponentCreated;
    Event<void(const HComponent&)> Component::OnComponentInitialized;
    Event<void(const HComponent&)> Component::OnComponentDestroyed;
    Event<void(const HComponent&)> Component::OnComponentEnabled;
    Event<void(const HComponent&)> Component::OnComponentDisabled;

    Component::Component(HSceneObject parent, UINT32 type)
        : Serializable(type)
        , _parent(std::move(parent))
    {
        SetName("Component");
    }

    void Component::SetSceneObject(HSceneObject& sceneObject)
    {
        _parent = sceneObject.GetNewHandleFromExisting();
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

    bool Component::IsDescendantOf(const HSceneObject& sceneObject)
    {
        for (const auto& componentSO : sceneObject->GetComponents())
        {
            if (GetUUID() == componentSO->GetUUID())
                return true;
        }

        for (const auto& childSO : sceneObject->GetChildren())
        {
            for (const auto& componentSO : childSO->GetComponents())
            {
                if (GetUUID() == componentSO->GetUUID())
                    return true;
            }

            if (childSO->GetNumChildren() > 0)
            {
                if (IsDescendantOf(childSO))
                    return true;
            }
        }

        return false;
    }

    bool Component::IsChildOf(const HSceneObject& sceneObject)
    {
        for (const auto& componentSO : sceneObject->GetComponents())
        {
            if (GetUUID() == componentSO->GetUUID())
                return true;
        }

        return false;
    }
}
