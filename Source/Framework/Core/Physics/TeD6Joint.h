#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    struct D6_JOINT_DESC;

    /**
     * Represents the most customizable type of joint. This joint type can be used to create all other built-in joint
     * types, and to design your own custom ones, but is less intuitive to use. Allows a specification of a linear
     * constraint (for example for slider), twist constraint (rotating around X) and swing constraint (rotating around Y and
     * Z). It also allows you to constrain limits to only specific axes or completely lock specific axes.
     */
    class TE_CORE_EXPORT D6Joint : public Joint
    {
    public:
        D6Joint(const D6_JOINT_DESC& desc) { }
        virtual ~D6Joint() = default;
    };

    /** Structure used for initializing a new D6Joint. */
    struct D6_JOINT_DESC
    { };
}
