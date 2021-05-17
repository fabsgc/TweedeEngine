#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSphericalJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a Spherical joint. */
    class BulletSphericalJoint : public SphericalJoint, public BulletJoint
    {
    public:
        BulletSphericalJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletSphericalJoint();

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
