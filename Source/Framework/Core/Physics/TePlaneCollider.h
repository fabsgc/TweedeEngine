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
        /** Determines the normal of the plane geometry.  */
        virtual void SetNormal(const Vector3& normal) = 0;

        /** @copydoc SetNormal */
        virtual const Vector3& GetNormal() const = 0;

        /**
         * Creates a new mesh collider.
         *
         * @param[in]	scene		Scene into which to add the collider to.
         * @param[in]	normal      Normal to the plane.
         * @param[in]	position	Position of the collider relative to its parent
         * @param[in]	rotation	Position of the collider relative to its parent
         */
        static SPtr<PlaneCollider> Create(PhysicsScene& scene, const Vector3& normal = Vector3(0.0f, 1.0f, 0.0f), 
            const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);

    protected:
        PlaneCollider();
    };
}
