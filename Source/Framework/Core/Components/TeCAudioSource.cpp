#include "Components/TeCAudioSource.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CAudioSource::CAudioSource()
        : Component(HSceneObject(), (UINT32)TID_CAudioSource)
    {
        SetName("AudioSource");
        SetFlag(Component::AlwaysRun, true);
    }

    CAudioSource::CAudioSource(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CAudioSource)
    {
        SetName("AudioSource");
        SetFlag(Component::AlwaysRun, true);
    }

    void CAudioSource::_instantiate()
    {
        // TODO
    }

    void CAudioSource::OnInitialized()
    {
        // TODO
        Component::OnInitialized();
    }

    void CAudioSource::OnEnabled()
    {
        // TODO
        Component::OnEnabled();
    }

    void CAudioSource::OnDisabled()
    {
        // TODO
        Component::OnDisabled();
    }

    void CAudioSource::OnTransformChanged(TransformChangedFlags flags)
    {
        // TODO
    }

    void CAudioSource::OnDestroyed()
    {
        // gSceneManager()._unbindActor(_internal); TODO
        Component::OnDestroyed();
        // _internal->Destroy(); // TODO
    }

    void CAudioSource::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CAudioSource>(c));
    }

    void CAudioSource::Clone(const HAudioSource& c)
    {
        Component::Clone(c.GetInternalPtr());
        // TODO
    }

    void CAudioSource::RestoreInternal()
    {
        // TODO
    }

    void CAudioSource::DestroyInternal()
    {
        // TODO
    }
}
