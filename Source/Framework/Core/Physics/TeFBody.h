#pragma once

#include "TeCorePrerequisites.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /** Provides common functionality used by all Body types. */
    class TE_CORE_EXPORT FBody : public Serializable
    {
    public:
        FBody(CoreType type);
        virtual ~FBody() = 0;
    };
}
