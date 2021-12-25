#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePatchSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{
    /** Bullet implementation of a PatchSoftBody. */
    class BulletPatchSoftBody : public PatchSoftBody, public BulletSoftBody
    {
    public:
        BulletPatchSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletPatchSoftBody();

        /** @copydoc Body::Update */
        void Update() override;

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

    private:

    };
}
