#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with HeightField geometry. */
    class TE_CORE_EXPORT HeightFieldCollider : public Collider
    {
    public:
        HeightFieldCollider() = default;
        ~HeightFieldCollider() = default;

        /**
         * Creates a new HeightField collider.
         *
         * @param[in]	scene		Scene into which to add the collider to.
         * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        static SPtr<HeightFieldCollider> Create(PhysicsScene& scene, const Vector3& position = Vector3::ZERO,
            const Quaternion& rotation = Quaternion::IDENTITY);
    };
}
