#include "TeOAAudio.h"
#include "Math/TeMath.h"

namespace te
{
    OAAudio::OAAudio()
    {
        bool enumeratedDevices;
        if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT") != ALC_FALSE)
        {
            enumeratedDevices = true;
        }
        else
        {
            enumeratedDevices = false;
        }
    }

    OAAudio::~OAAudio()
    { }
}
