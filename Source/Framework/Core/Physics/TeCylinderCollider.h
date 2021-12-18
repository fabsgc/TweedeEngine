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
        CylinderCollider();
        ~CylinderCollider() = default;

        /** Determines the extents (half size) of the geometry of the cylinder. */
        virtual void SetExtents(const Vector3& extents) = 0;

        /** @copydoc SetExtents */
        virtual Vector3 GetExtents() const = 0;

        /**
         * Creates a new cylinder collider.
         *
         * @param[in]	scene		Scene into which to add the collider to.
         * @param[in]	extents		Extents (half size) of the cylinder.
		 * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        static SPtr<CylinderCollider> Create(PhysicsScene& scene, const Vector3& extents = Vector3::ZERO, 
            const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);
    };
}
