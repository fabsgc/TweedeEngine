#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudio.h"
#include <AL/alc.h>

namespace te
{
    /** Global manager for the audio implementation using OpenAL as the backend. */
    class OAAudio : public Audio
    {
    public:
        OAAudio();
        virtual ~OAAudio();
    };
}
