#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with mesh geometry. */
    class TE_CORE_EXPORT MeshCollider : public Collider
    {
    public:
        MeshCollider() = default;
        ~MeshCollider() = default;

        /**
         * Creates a new mesh collider.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<MeshCollider> Create(PhysicsScene& scene);
    };
}
