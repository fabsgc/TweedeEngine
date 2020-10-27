#pragma once

#include "TeCorePrerequisites.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /**
     * A native script is the base class of all scripts that can be written, loaded and executed inside the engine
     */
    class TE_CORE_EXPORT NativeScript : public Serializable
    {
    public:
        NativeScript();
        ~NativeScript();
    };
}
