#include "Audio/TeAudio.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Audio)

    Audio& gAudio()
    {
        return Audio::Instance();
    }
}
