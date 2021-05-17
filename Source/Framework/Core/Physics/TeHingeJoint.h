#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    /**
     * Hinge joint removes all but a single rotation degree of freedom from its two attached bodies (for example a door
     * hinge).
     */
    class TE_CORE_EXPORT HingeJoint : public Joint
    {
    public:
        HingeJoint() { }
        virtual ~HingeJoint() = default;

        /**
         * Creates a new hinge joint.
         *
         * @param[in]	scene		Scene to which to add the joint.
         * @param[in]	desc		Settings describing the joint.
         */
        static SPtr<HingeJoint> Create(PhysicsScene& scene);
    };
}
