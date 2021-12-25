#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeRopeSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{
    /** Bullet implementation of a RopeSoftBody. */
    class BulletRopeSoftBody : public RopeSoftBody, public BulletSoftBody
    {
    public:
        BulletRopeSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletRopeSoftBody();

        /** @copydoc Body::Update */
        void Update() override;

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

    private:
        
    };
}
