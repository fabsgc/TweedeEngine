#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    /**
     * Represents the most customizable type of joint. This joint type can be used to create all other built-in joint
     * types, and to design your own custom ones, but is less intuitive to use. Allows a specification of a linear
     * constraint (for example for slider), twist constraint (rotating around X) and swing constraint (rotating around Y and
     * Z). It also allows you to constrain limits to only specific axes or completely lock specific axes.
     */
    class TE_CORE_EXPORT D6Joint : public Joint
    {
    public:
        D6Joint() { }
        ~D6Joint() = default;

        /** @copydoc Joint::Update */
        virtual void Update() = 0;

        /**
         * Creates a new d6 joint.
         *
         * @param[in]	scene		Scene to which to add the joint.
         * @param[in]	desc		Settings describing the joint.
         */
        static SPtr<D6Joint> Create(PhysicsScene& scene);
    };
}
