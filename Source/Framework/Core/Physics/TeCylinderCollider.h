#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with cylinder geometry. */
    class TE_CORE_EXPORT CylinderCollider : public Collider
    {
    public:
        CylinderCollider() = default;
        ~CylinderCollider() = default;

        /**
         * Creates a new cylinder collider.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<CylinderCollider> Create(PhysicsScene& scene);
    };
}
