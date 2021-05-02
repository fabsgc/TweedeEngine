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

    void CAudioSource::Initialize()
    {
        RestoreInternal();
        Component::Initialize();
    }

    void CAudioSource::SetClip(const HAudioClip& clip)
    {
        if (_audioClip == clip)
            return;

        _audioClip = clip;

        if (_internal != nullptr)
            _internal->SetClip(clip);
    }

    void CAudioSource::SetVolume(float volume)
    {
        if (_volume == volume)
            return;

        _volume = volume;

        if (_internal != nullptr)
            _internal->SetVolume(volume);
    }

    void CAudioSource::SetPitch(float pitch)
    {
        if (_pitch == pitch)
            return;

        _pitch = pitch;

        if (_internal != nullptr)
            _internal->SetPitch(pitch);
    }

    void CAudioSource::SetIsLooping(bool loop)
    {
        if (_loop == loop)
            return;

        _loop = loop;

        if (_internal != nullptr)
            _internal->SetIsLooping(loop);
    }

    void CAudioSource::SetPriority(UINT32 priority)
    {
        if (_priority == priority)
            return;

        _priority = priority;

        if (_internal != nullptr)
            _internal->SetPriority(priority);
    }

    void CAudioSource::SetMinDistance(float distance)
    {
        if (_minDistance == distance)
            return;

        _minDistance = distance;

        if (_internal != nullptr)
            _internal->SetMinDistance(distance);
    }

    void CAudioSource::SetAttenuation(float attenuation)
    {
        if (_attenuation == attenuation)
            return;

        _attenuation = attenuation;

        if (_internal != nullptr)
            _internal->SetAttenuation(attenuation);
    }

    void CAudioSource::Play()
    {
        if (_internal != nullptr)
            _internal->Play();
    }

    void CAudioSource::Pause()
    {
        if (_internal != nullptr)
            _internal->Pause();
    }

    void CAudioSource::Stop()
    {
        if (_internal != nullptr)
            _internal->Stop();
    }

    void CAudioSource::SetTime(float position)
    {
        if (_internal != nullptr)
            _internal->SetTime(position);
    }

    float CAudioSource::GetTime() const
    {
        if (_internal != nullptr)
            return _internal->GetTime();

        return 0.0f;
    }

    AudioSourceState CAudioSource::GetState() const
    {
        if (_internal != nullptr)
            return _internal->GetState();

        return AudioSourceState::Stopped;
    }

    void CAudioSource::OnInitialized()
    {
        Component::OnInitialized();
    }

    void CAudioSource::OnEnabled()
    {
        RestoreInternal();

        if (_playOnStart)
            Play();

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
        DestroyInternal();
        Component::OnDestroyed();
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
        
        _audioClip = c->_audioClip;
        _volume = c->_volume;
        _pitch = c->_pitch;
        _loop = c->_loop;
        _priority = c->_priority;
        _minDistance = c->_minDistance;
        _attenuation = c->_attenuation;
    }

    void CAudioSource::RestoreInternal()
    {
        if (_internal == nullptr)
            _internal = AudioSource::Create();

        _internal->SetClip(_audioClip);
        _internal->SetVolume(_volume);
        _internal->SetPitch(_pitch);
        _internal->SetIsLooping(_loop);
        _internal->SetPriority(_priority);
        _internal->SetMinDistance(_minDistance);
        _internal->SetAttenuation(_attenuation);

        UpdateTransform();
    }

    void CAudioSource::DestroyInternal()
    {
        _internal = nullptr;
    }

    void CAudioSource::UpdateTransform()
    {
        const Transform& tfrm = SO()->GetTransform();

        _internal->SetTransform(tfrm);
        _internal->SetVelocity(_velocity);
    }
}
