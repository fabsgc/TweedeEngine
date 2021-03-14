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
        // TODO
    }

    void OAAudioListener::SetVelocity(const Vector3& velocity)
    {
        // TODO
    }

    void OAAudioListener::Rebuild()
    {

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
