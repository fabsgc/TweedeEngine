#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeCylinderCollider.h"
#include "TeBulletCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a CylinderCollider. */
    class BulletCylinderCollider : public CylinderCollider, public BulletCollider
    {
    public:
        BulletCylinderCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
            const Quaternion& rotation, const Vector3& extents);
        virtual ~BulletCylinderCollider();

        /** @copydoc CylinderCollider::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc CylinderCollider::SetExtents */
        void SetExtents(const Vector3& extents) override;

        /** @copydoc CylinderCollider::GetExtents */
        Vector3 GetExtents() const override { return _extents; }

    private:
        /** @copydoc BulletCollider::UpdateCollider */
        void UpdateCollider() override;

    private:
        btCylinderShape* _shape = nullptr;
        Vector3 _extents;
    };
}
