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

        /** @copydoc MeshCollider::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc MeshCollider::SetMesh */
        void SetMesh(const HPhysicsMesh& mesh) override;

        /** @copydoc MeshCollider::SetType */
        void SetCollisionType(PhysicsMeshType type) override;

    private:
        /** Create shape using current parameters */
        void UpdateShape();

        /** @copydoc MeshCollider::OnMeshChanged */
        void OnMeshChanged() override;

    private:
        btCollisionShape* _shape = nullptr;
    };
}
