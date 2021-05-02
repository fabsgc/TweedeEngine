#include "Components/TeCAudioListener.h"

#include "Scene/TeSceneManager.h"
#include "Utility/TeTime.h"

namespace te
{
    CAudioListener::CAudioListener()
        : Component(HSceneObject(), (UINT32)TID_CAudioListener)
    {
        SetName("AudioListener");
        _notifyFlags = TCF_Transform;
    }

    CAudioListener::CAudioListener(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CAudioListener)
    {
        SetName("AudioListener");
        _notifyFlags = TCF_Transform;
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

    void CAudioListener::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CAudioListener>(c));
    }

    void CAudioListener::Clone(const HAudioListener& c)
    {
        Component::Clone(c.GetInternalPtr());

        _lastPosition = c->_lastPosition;
        _velocity = c->_velocity;
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
