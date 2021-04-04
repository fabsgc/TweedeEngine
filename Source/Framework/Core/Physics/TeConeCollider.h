#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with cone geometry. */
    class TE_CORE_EXPORT ConeCollider : public Collider
    {
    public:
        ConeCollider() = default;
        ~ConeCollider() = default;

        /**
         * Creates a new capsule collider.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<ConeCollider> Create(PhysicsScene& scene);
    };
}
