#include "Components/TeCAudioSource.h"

#include "Scene/TeSceneManager.h"
#include "Utility/TeTime.h"
#include "Serialization/TeUtility.h"
#include "Resources/TeResourceManager.h"
#include "Audio/TeAudioClip.h"
#include "../TeCoreApplication.h"

namespace te
{
    CAudioSource::CAudioSource()
        : Component(HSceneObject(), CoreType::TID_CAudioSource)
    {
        SetName("AudioSource");
        SetNotifyFlags(TCF_Parent | TCF_Transform);
        SetFlag(Component::AlwaysRun, true);
    }

    CAudioSource::CAudioSource(const HSceneObject& parent)
        : Component(parent, CoreType::TID_CAudioSource)
    {
        SetName("AudioSource");
        SetNotifyFlags(TCF_Parent | TCF_Transform);
        SetFlag(Component::AlwaysRun, true);
    }

    CAudioSource::~CAudioSource()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
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

    void CAudioSource::SetIsPlay3D(bool play3D)
    {
        if (_play3D == play3D)
            return;

        _play3D = play3D;

        if (_internal != nullptr)
            _internal->SetIsPlay3D(play3D);

        UpdateTransform();
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

    void CAudioSource::OnEnabled()
    {
        RestoreInternal();

        if (_playOnStart && gCoreApplication().GetState().IsFlagSet(ApplicationState::Game))
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

    bool CAudioSource::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CAudioSource>(c), suffix);
    }

    bool CAudioSource::Clone(const HAudioSource& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            _audioClip = c->_audioClip;
            _volume = c->_volume;
            _pitch = c->_pitch;
            _loop = c->_loop;
            _priority = c->_priority;
            _minDistance = c->_minDistance;
            _attenuation = c->_attenuation;
            _playOnStart = c->_playOnStart;

            return true;
        }

        return false;
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
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
        _internal = nullptr;
    }

    void CAudioSource::UpdateTransform()
    {
        const Transform& tfrm = SO()->GetTransform();

        _internal->SetTransform(tfrm);
        _internal->SetVelocity(_velocity);
    }

    void CAudioSource::ExportJson(nlohmann::json& document) const
    {
        Component::ExportJson(document);

        document["type"] = GetComponentType();

        if (_internal)
        {
            document["clip"] = _audioClip.IsLoaded() ? serialization::GetResourceName(_audioClip.Get()) : "";
            document["volume"] = _volume;
            document["pitch"] = _pitch;
            document["loop"] = _loop;
            document["priority"] = _priority;
            document["minDistance"] = _minDistance;
            document["attenuation"] = _attenuation;
            document["play3D"] = _play3D;
            document["playOnStart"] = _playOnStart;

            _lastPosition.ExportJson(document["lastPosition"]);
            _velocity.ExportJson(document["velocity"]);
        }
    }

    bool CAudioSource::ImportJson(const nlohmann::json& document, CAudioSource& audioSource)
    {
        Component::ImportJson(document, audioSource);

        if (document.contains("clip"))
        {
            HAudioClip clip = static_resource_cast<AudioClip>(gResourceManager().Get(serialization::GetResourceUUID(document["clip"].get<String>())));
            audioSource.SetClip(clip);
        }

        if (document.contains("volume"))
            audioSource.SetVolume(document["volume"].get<float>());
        if (document.contains("pitch"))
            audioSource.SetPitch(document["pitch"].get<float>());
        if (document.contains("loop"))
            audioSource.SetIsLooping(document["loop"].get<bool>());
        if (document.contains("priority"))
            audioSource.SetPriority(document["priority"].get<int32_t>());
        if (document.contains("minDistance"))
            audioSource.SetMinDistance(document["minDistance"].get<float>());
        if (document.contains("attenuation"))
            audioSource.SetAttenuation(document["attenuation"].get<float>());
        if (document.contains("play3D"))
            audioSource.SetIsPlay3D(document["play3D"].get<bool>());
        if (document.contains("playOnStart"))
            audioSource.SetPlayOnStart(document["playOnStart"].get<bool>());

        if (document.contains("lastPosition"))
            audioSource._lastPosition = Vector3::ImportJson(document["lastPosition"]);
        if (document.contains("velocity"))
            audioSource._velocity = Vector3::ImportJson(document["velocity"]);

        return true;
    }
}
