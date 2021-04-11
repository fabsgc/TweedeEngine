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

        /** Determines the extents (half size) of the geometry of the box. */
        virtual void SetExtents(const Vector3& extents) = 0;

        /** @copydoc SetExtents() */
        virtual Vector3 GetExtents() const = 0;

        /**
         * Creates a new box collider.
         *
         * @param[in]	scene		Scene into which to add the collider to.
         * @param[in]	extents		Extents (half size) of the box.
		 * @param[in]	position	Center of the box.
		 * @param[in]	rotation	Rotation of the box.
         */
        static SPtr<BoxCollider> Create(PhysicsScene& scene, const Vector3& extents = Vector3::ZERO,
            const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);
    };
}
