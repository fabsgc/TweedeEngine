#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeD6Joint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a D6 joint. */
    class BulletD6Joint : public D6Joint, public BulletJoint
    {
    public:
        BulletD6Joint(BulletPhysics* physics, BulletScene* scene);
        ~BulletD6Joint();

        /** @copydoc Joint::Update */
        void Update() override { }

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
