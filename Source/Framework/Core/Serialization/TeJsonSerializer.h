#pragma once

#include "TeCorePrerequisites.h"

#include "Serialization/TeSerializer.h"

namespace te
{
    class TE_CORE_EXPORT JsonSerializer : public Serializer
    {
    public:
        JsonSerializer() = default;
        ~JsonSerializer() = default;
    };
}
