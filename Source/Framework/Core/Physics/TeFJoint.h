#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{

    /** Provides common functionality used by all Joint types. */
    class TE_CORE_EXPORT FJoint
    {
    public:
        FJoint(const JOINT_DESC& desc) { }
        virtual ~FJoint() = default;
    };
};
