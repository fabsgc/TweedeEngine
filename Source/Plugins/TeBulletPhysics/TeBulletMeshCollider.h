#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeMeshCollider.h"
#include "TeBulletPhysics.h"

namespace te
{
    /** Bullet implementation of a MeshCollider. */
    class BulletMeshCollider : public MeshCollider
    {
    public:
        BulletMeshCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position, const Quaternion& rotation);
        ~BulletMeshCollider();

        /** @copydoc SphereCollider::SetScale() */
        void SetScale(const Vector3& scale) override;

    private:
        /** Create shape using current parameters */
        void UpdateShape();

    private:
        btConvexHullShape* _shape = nullptr;
    };
}
