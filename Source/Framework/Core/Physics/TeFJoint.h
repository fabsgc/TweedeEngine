#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Specifies first or second body referenced by a Joint. */
    enum class JointBody
    {
        Target, /**< Body the joint is influencing. */
        Anchor /**< Body the joint is attached to (if any). */
    };

    /** Provides common functionality used by all Joint types. */
    class TE_CORE_EXPORT FJoint
    {
    public:
        FJoint() { }
        virtual ~FJoint() = default;
    };
};
