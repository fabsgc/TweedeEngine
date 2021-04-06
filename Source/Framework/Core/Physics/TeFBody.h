#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{

    /** Provides common functionality used by all Body types. */
    class TE_CORE_EXPORT FBody
    {
    public:
        virtual ~FBody() = default;
    };
};
