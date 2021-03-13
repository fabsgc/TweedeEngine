#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudioSource.h"

namespace te
{
    /** OpenAL implementation of an AudioSource. */
    class OAAudioSource : public AudioSource
    {
    public:
        OAAudioSource();
        virtual ~OAAudioSource();

        /** @copydoc AudioSource::SetTime */
        void SetTime(float time) override;

        /** @copydoc AudioSource::GetTime */
        float GetTime() const override;

        /** @copydoc AudioSource::Play */
        void Play() override;

        /** @copydoc AudioSource::Pause */
        void Pause() override;

        /** @copydoc AudioSource::Stop */
        void Stop() override;

        /** @copydoc AudioSource::GetState */
        AudioSourceState GetState() const override;

    private:
        friend class OOAudio;

    private:
        Vector<UINT32> _sourceIDs;
    };
}
