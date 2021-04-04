#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with plane geometry. */
    class TE_CORE_EXPORT PlaneCollider : public Collider
    {
    public:
        PlaneCollider() = default;
        ~PlaneCollider() = default;

        /**
         * Creates a new mesh collider.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<PlaneCollider> Create(PhysicsScene& scene);
    };
}
