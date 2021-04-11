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
         * @param[in]	scene		Scene into which to add the collider to.
         */
        static SPtr<ConeCollider> Create(PhysicsScene& scene);
    };
}
