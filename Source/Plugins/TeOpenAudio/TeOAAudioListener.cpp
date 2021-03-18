#include "TeOAAudioListener.h"
#include "TeOAAudio.h"
#include "AL/al.h"

namespace te
{
    OAAudioListener::OAAudioListener()
    { 
        gOAAudio()._registerListener(this);
        Rebuild();
    }

    OAAudioListener::~OAAudioListener()
    { 
        gOAAudio()._unregisterListener(this);
    }

    void OAAudioListener::SetTransform(const Transform& transform)
    {
        AudioListener::SetTransform(transform);

        std::array<float, 6> orientation = GetOrientation();
        auto& contexts = gOAAudio()._getContexts();

        if (contexts.size() > 1) // If only one context is available it is guaranteed it is always active, so we can avoid setting it
        {
            auto context = gOAAudio()._getContext(this);
            alcMakeContextCurrent(context);
        }

        UpdatePosition();
        UpdateOrientation(orientation);
    }

    void OAAudioListener::SetVelocity(const Vector3& velocity)
    {
        AudioListener::SetVelocity(velocity);

        auto& contexts = gOAAudio()._getContexts();
        if (contexts.size() > 1)
        {
            auto context = gOAAudio()._getContext(this);
            alcMakeContextCurrent(context);
        }

        UpdateVelocity();
    }

    void OAAudioListener::Rebuild()
    {
        auto contexts = gOAAudio()._getContexts();

        float globalVolume = gAudio().GetVolume();
        std::array<float, 6> orientation = GetOrientation();

        if (contexts.size() > 1)
        {
            auto context = gOAAudio()._getContext(this);
            alcMakeContextCurrent(context);
        }

        UpdateDopplerFactor();
        UpdatePosition();
        UpdateOrientation(orientation);
        UpdateVelocity();
        UpdateVolume(globalVolume);
    }

    std::array<float, 6> OAAudioListener::GetOrientation() const
    {
        Vector3 direction = GetTransform().GetForward();
        Vector3 up = GetTransform().GetUp();

        return
        { {
            direction.x,
            direction.y,
            direction.z,
            up.x,
            up.y,
            up.z
        } };
    }

    void OAAudioListener::UpdateDopplerFactor()
    {
        alDopplerFactor(1.0);
        alDopplerVelocity(343.0f);
    }

    void OAAudioListener::UpdatePosition()
    {
        Vector3 position = GetTransform().GetPosition();
        alListener3f(AL_POSITION, position.x, position.y, position.z);
    }

    void OAAudioListener::UpdateOrientation(const std::array<float, 6>& orientation)
    {
        alListenerfv(AL_ORIENTATION, orientation.data());
    }

    void OAAudioListener::UpdateVelocity()
    {
        alListener3f(AL_VELOCITY, _velocity.x, _velocity.y, _velocity.z);
    }

    void OAAudioListener::UpdateVolume(float volume)
    {
        alListenerf(AL_GAIN, volume);
    }
}
