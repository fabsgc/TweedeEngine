#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with capsule geometry. */
    class TE_CORE_EXPORT CapsuleCollider : public Collider
    {
    public:
        CapsuleCollider() = default;
        ~CapsuleCollider() = default;

        /**
         * Creates a new capsule collider.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<CapsuleCollider> Create(PhysicsScene& scene);
    };
}
