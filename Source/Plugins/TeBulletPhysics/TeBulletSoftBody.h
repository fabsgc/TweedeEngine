#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Bullet implementation of a SoftBody. */
    class BulletSoftBody
    {
    protected:
        friend class BulletFSoftBody;

        /** Add RigidBody to world */
        virtual void AddToWorld() = 0;

        /** Release Body from simulation */
        virtual void Release() = 0;

        /** Remove RigidBody from world */
        virtual void RemoveFromWorld() = 0;

        /** Update kinematic bullet flag */
        virtual void UpdateKinematicFlag() const = 0;

        /** Enable or disable CCD for this body */
        virtual void UpdateCCDFlag() const = 0;

        /** Activate btRigidBody */
        virtual void Activate() const = 0;

        /** Check if btRigidBody is activated */
        virtual bool IsActivated() const = 0;
    };
}
