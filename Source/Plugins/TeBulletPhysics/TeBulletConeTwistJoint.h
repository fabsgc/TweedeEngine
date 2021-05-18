#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeConeTwistJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a ConeTwist joint. */
    class BulletConeTwistJoint : public ConeTwistJoint, public BulletJoint
    {
    public:
        BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletConeTwistJoint();

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
