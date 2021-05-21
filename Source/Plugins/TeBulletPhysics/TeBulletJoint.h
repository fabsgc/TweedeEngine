#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "TeBulletRigidBody.h"
#include "Physics/TeJoint.h"

namespace te
{
    /** Bullet implementation of joint. */
    class BulletJoint
    {
    public:
        BulletJoint(BulletPhysics* physics, BulletScene* scene, Joint* joint);
        ~BulletJoint();

    protected:
        /** Build internal bullet representation of a joint */
        virtual void BuildJoint() = 0;

        /** Update the internal representation of a joint */
        virtual void UpdateJoint() = 0;

        /** Release the internal representation of a joint from the world */
        virtual void ReleaseJoint() = 0;

        /** Returns associated btJoint */
        btTypedConstraint* GetJoint() { return _btJoint; }

        /** Common method to retrieve btRigidBody from Joint::_bodies */
        btRigidBody* GetBtRigidBody(BodyInfo* info);

    protected:
        friend class BulletRigidBody;
        friend class Joint;

        BulletPhysics* _physics;
        BulletScene* _scene;

        btTypedConstraint* _btJoint;
        Joint* _joint;
    };
}
