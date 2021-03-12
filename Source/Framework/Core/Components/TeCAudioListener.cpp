#include "Components/TeCAudioListener.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CAudioListener::CAudioListener()
        : Component(HSceneObject(), (UINT32)TID_CAudioListener)
    {
        SetName("AudioListener");
        SetFlag(Component::AlwaysRun, true);
    }

    CAudioListener::CAudioListener(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CAudioListener)
    {
        SetName("AudioListener");
        SetFlag(Component::AlwaysRun, true);
    }

    void CAudioListener::_instantiate()
    {
        // TODO
    }

    void CAudioListener::OnInitialized()
    {
        // TODO
        Component::OnInitialized();
    }

    void CAudioListener::OnEnabled()
    {
        // TODO
        Component::OnEnabled();
    }

    void CAudioListener::OnDisabled()
    {
        // TODO
        Component::OnDisabled();
    }

    void CAudioListener::OnTransformChanged(TransformChangedFlags flags)
    {
        // TODO
    }

    void CAudioListener::OnDestroyed()
    {
        // gSceneManager()._unbindActor(_internal); TODO
        Component::OnDestroyed();
        // _internal->Destroy(); // TODO
    }

    void CAudioListener::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CAudioListener>(c));
    }

    void CAudioListener::Clone(const HAudioListener& c)
    {
        Component::Clone(c.GetInternalPtr());
        // TODO
    }

    void CAudioListener::RestoreInternal()
    {
        // TODO
    }

    void CAudioListener::DestroyInternal()
    {
        // TODO
    }
}
