#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with box geometry. */
    class TE_CORE_EXPORT BoxCollider : public Collider
    {
    public:
        BoxCollider() = default;
        ~BoxCollider() = default;

        /**
         * Creates a new box collider.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<BoxCollider> Create(PhysicsScene& scene);
    };
}
