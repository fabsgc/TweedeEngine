#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with sphere geometry. */
    class TE_CORE_EXPORT SphereCollider : public Collider
    {
    public:
        SphereCollider() = default;
        ~SphereCollider() = default;

        /**
         * Creates a new sphere collider.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<SphereCollider> Create(PhysicsScene& scene);
    };
}
