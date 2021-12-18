#pragma once

#include "Physics/TePhysicsCommon.h"

namespace te
{
    /** Provides common functionality used by all Body types. */
    class TE_CORE_EXPORT FBody : public Serializable
    {
    public:
        FBody();
        virtual ~FBody() = default;
    };
}
