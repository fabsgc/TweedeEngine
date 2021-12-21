#include "Components/TeCAudioListener.h"

#include "Scene/TeSceneManager.h"
#include "Utility/TeTime.h"

namespace te
{
    CAudioListener::CAudioListener()
        : Component(HSceneObject(), (UINT32)TID_CAudioListener)
    {
        SetName("AudioListener");
        SetNotifyFlags(TCF_Transform | TCF_Parent);
        SetFlag(Component::AlwaysRun, true);
    }

    CAudioListener::CAudioListener(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CAudioListener)
    {
        SetName("AudioListener");
        SetNotifyFlags(TCF_Transform | TCF_Parent);
        SetFlag(Component::AlwaysRun, true);
    }

    void CAudioListener::Initialize()
    {
        RestoreInternal();
        Component::Initialize();
    }

    void CAudioListener::Update()
    {
        const Vector3 worldPos = SO()->GetTransform().GetPosition();

        const float frameDelta = gTime().GetFrameDelta();
        if (frameDelta > 0.0f)
            _velocity = (worldPos - _lastPosition) / frameDelta;
        else
            _velocity = Vector3::ZERO;

        _lastPosition = worldPos;
    }

    void CAudioListener::OnInitialized()
    {
        Component::OnInitialized();
    }

    void CAudioListener::OnEnabled()
    {
        RestoreInternal();
        Component::OnEnabled();
    }

    void CAudioListener::OnDisabled()
    {
        DestroyInternal();
        Component::OnDisabled();
    }

    void CAudioListener::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & (TCF_Parent | TCF_Transform)) != 0)
            UpdateTransform();
    }

    void CAudioListener::OnDestroyed()
    {
        DestroyInternal();
        Component::OnDestroyed();
    }

    bool CAudioListener::Clone(const HAudioListener& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            _lastPosition = c->_lastPosition;
            _velocity = c->_velocity;

            return true;
        }

        return false;
    }

    void CAudioListener::RestoreInternal()
    {
        if (_internal == nullptr)
            _internal = AudioListener::Create();

        UpdateTransform();
    }

    void CAudioListener::DestroyInternal()
    {
        _internal = nullptr;
    }

    void CAudioListener::UpdateTransform()
    {
        const Transform& tfrm = SO()->GetTransform();

        _internal->SetTransform(tfrm);
        _internal->SetVelocity(_velocity);
    }
}
