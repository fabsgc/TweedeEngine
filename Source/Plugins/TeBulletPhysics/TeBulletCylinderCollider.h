#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeCylinderCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a CylinderCollider. */
    class BulletCylinderCollider : public CylinderCollider
    {
    public:
        BulletCylinderCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
            const Quaternion& rotation, const Vector3& extents);
        ~BulletCylinderCollider();

        /** @copydoc CylinderCollider::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc CylinderCollider::SetExtents */
        void SetExtents(const Vector3& extents) override;

        /** @copydoc CylinderCollider::GetExtents */
        Vector3 GetExtents() const override { return _extents; }

    private:
        /** Create shape using current parameters */
        void UpdateShape();

    private:
        btCylinderShape* _shape = nullptr;
        Vector3 _extents;
    };
}
