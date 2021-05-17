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

        /** Determine the radius of the capsule geometry */
        virtual void SetRadius(float radius) = 0;

        /** @copydoc SetRadius */
        virtual float GetRadius() const = 0;

        /** Determine the height of the capsule geometry */
        virtual void SetHeight(float height) = 0;

        /** @copydoc SetHeight */
        virtual float GetHeight() const = 0;

        /**
         * Creates a new capsule collider.
         *
         * @param[in]	scene		Scene into which to add the collider to.
         * @param[in]	radius		Radius of the capsule
         * @param[in]	height		Height of the capsule
		 * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        static SPtr<CapsuleCollider> Create(PhysicsScene& scene, float radius = 1.0f, float height = 1.0f, 
            const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);
    };
}
