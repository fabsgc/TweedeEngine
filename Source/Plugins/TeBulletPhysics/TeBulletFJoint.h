#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFJoint.h"

namespace te
{
    /** Bullet implementation of an FJoint. */
    class BulletFJoint : public FJoint
    {
    public:
        BulletFJoint(BulletPhysics* physics, BulletScene* scene, const JOINT_DESC& desc);
        ~BulletFJoint();

        /** Set current btTypedConstraint */
        void SetJoint(btTypedConstraint* joint) { _joint = joint; }

        /** Return current btCollisionShape */
        const auto& GetJoint() const { return _joint; }

    protected:
        BulletPhysics* _physics;
        BulletScene* _scene;

        btTypedConstraint* _joint;
    };
}
