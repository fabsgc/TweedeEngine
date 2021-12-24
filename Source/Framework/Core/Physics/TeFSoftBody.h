#pragma once

#include "Physics/TePhysicsCommon.h"
#include "TeFBody.h"

namespace te
{
    /** Provides common functionality used by all SoftBody types. */
    class TE_CORE_EXPORT FSoftBody : public FBody
    {
    public:
        FSoftBody();
        virtual ~FSoftBody() = 0;
    };
}
