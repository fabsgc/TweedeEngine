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
    };
}
