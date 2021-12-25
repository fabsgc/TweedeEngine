#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeEllipsoidSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{
    /** Bullet implementation of a EllipsoidSoftBody. */
    class BulletEllipsoidSoftBody : public EllipsoidSoftBody, public BulletSoftBody
    {
    public:
        BulletEllipsoidSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletEllipsoidSoftBody();

        /** @copydoc Body::Update */
        void Update() override;

    private:
        /** @copydoc BulletSoftBody::AddToWorld */
        void AddToWorld() override;

    private:

    };
}
