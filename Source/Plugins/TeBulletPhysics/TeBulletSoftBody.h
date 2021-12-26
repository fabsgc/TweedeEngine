#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Bullet implementation of a SoftBody. */
    class BulletSoftBody
    {
    public:
        BulletSoftBody(BulletPhysics* physics, BulletScene* scene);

    protected:
        friend class BulletFSoftBody;

        /** Common part used by all soft bodies */
        void AddToWorldInternal(FBody* body);

        /** Add RigidBody to world */
        virtual void AddToWorld() = 0;

        /** Release Body from simulation */
        virtual void Release(FBody* fBody);

        /** Remove RigidBody from world */
        virtual void RemoveFromWorld(FBody* fBody);

        /** Update kinematic bullet flag */
        virtual void UpdateKinematicFlag(FBody* fBody) const;

        /** Enable or disable CCD for this body */
        virtual void UpdateCCDFlag(FBody* fBody) const;

        /** Activate btRigidBody */
        virtual void Activate(FBody* fBody) const;

        /** Check if btRigidBody is activated */
        virtual bool IsActivated(FBody* fBody) const;

    protected:
        btSoftBody* _softBody;
        BulletPhysics* _physics;
        BulletScene* _scene;

        bool _isDirty = true;
        bool _inWorld = false;
    };
}
