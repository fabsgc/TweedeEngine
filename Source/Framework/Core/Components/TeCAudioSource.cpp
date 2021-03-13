#include "Components/TeCAudioSource.h"
#include "Scene/TeSceneManager.h"
#include "Utility/TeTime.h"

namespace te
{
    CAudioSource::CAudioSource()
        : Component(HSceneObject(), (UINT32)TID_CAudioSource)
    {
        SetName("AudioSource");
        _notifyFlags = TCF_Transform;
    }

    CAudioSource::CAudioSource(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CAudioSource)
    {
        SetName("AudioSource");
        _notifyFlags = TCF_Transform;
    }

    void CAudioSource::SetClip(const HAudioClip& clip)
    {
        if (_audioClip == clip)
            return;

        _audioClip = clip;

        if (_internal != nullptr)
            _internal->SetClip(clip);
    }

    void CAudioSource::_instantiate()
    { }

    void CAudioSource::OnInitialized()
    {
        Component::OnInitialized();
    }

    void CAudioSource::OnEnabled()
    {
        RestoreInternal();

        //if (_playOnStart)
        //    Play();
        // TODO

        Component::OnEnabled();
    }

    void CAudioSource::OnDisabled()
    {
        DestroyInternal();
        Component::OnDisabled();
    }

    void CAudioSource::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & (TCF_Parent | TCF_Transform)) != 0)
            UpdateTransform();
    }

    void CAudioSource::OnDestroyed()
    {
        Component::OnDestroyed();
        DestroyInternal();
    }

    void CAudioSource::Update()
    {
        const Vector3 worldPos = SO()->GetTransform().GetPosition();

        const float frameDelta = gTime().GetFrameDelta();
        if (frameDelta > 0.0f)
            _velocity = (worldPos - _lastPosition) / frameDelta;
        else
            _velocity = Vector3::ZERO;

        _lastPosition = worldPos;
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
        if (_internal == nullptr)
            _internal = AudioSource::Create();

        // Note: Merge into one call to avoid many virtual function calls
        _internal->SetClip(_audioClip);
        // TODO

        UpdateTransform();
    }

    void CAudioSource::DestroyInternal()
    {
        // This should release the last reference and destroy the internal listener
        _internal = nullptr;
    }

    void CAudioSource::UpdateTransform()
    {
        _internal->SetTransform(SO()->GetTransform());
        _internal->SetVelocity(_velocity);
    }
}
