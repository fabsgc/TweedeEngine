#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudioListener.h"

namespace te
{
    /** OpenAL implementation of an AudioListener. */
    class OAAudioListener : public AudioListener
    {
    public:
        OAAudioListener();
        virtual ~OAAudioListener();

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

        /** @copydoc AudioListener::SetVelocity */
        void SetVelocity(const Vector3& velocity) override;

    private:
        friend class OAAudio;

        /** Re-applies stored properties to the listener. */
        void Rebuild();

        /** Returns forward and up direction as a single vector. */
        inline std::array<float, 6> GetOrientation() const;

        /** Updates internal doppler effect for this listener */
        inline void UpdateDopplerFactor();

        /** Updates internal position of the listener. */
        inline void UpdatePosition();

        /** Updates internal forward and up directions of the listener. */
        inline void UpdateOrientation(const std::array<float, 6>& orientation);

        /** Updates internal velocity of the listener. */
        inline void UpdateVelocity();

        /** Updates internal volume of the listener. */
        inline void UpdateVolume(float volume);

    };
}
