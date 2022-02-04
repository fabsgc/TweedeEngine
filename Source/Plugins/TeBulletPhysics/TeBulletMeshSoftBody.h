#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeMeshSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{
    /** Bullet implementation of a MeshSoftBody. */
    class BulletMeshSoftBody : public MeshSoftBody, public BulletSoftBody
    {
    public:
        BulletMeshSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        virtual ~BulletMeshSoftBody();

        /** @copydoc Body::Update */
        void Update() override;

        /** @copydoc MeshSoftBody::SetMesh */
        void SetMesh(const HPhysicsMesh& mesh) override;

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

    private:

    };
}
