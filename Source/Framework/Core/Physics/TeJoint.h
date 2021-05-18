#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeFJoint.h"
#include "Utility/TeEvent.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

#include <cfloat>

namespace te
{
    struct BodyInfo
    {
        Body* BodyElt = nullptr;
        Vector3 Position = Vector3::ZERO;
        Quaternion Rotation = Quaternion::IDENTITY;
    };

    /**
     * Base class for all Joint types. Joints constrain how two bodies move relative to one another (for example a door
     * hinge). One of the bodies in the joint must always be movable (non-kinematic).
     */
    class TE_CORE_EXPORT Joint
    {
    public:
        Joint() = default;
        virtual ~Joint() = default;

        /** Allows to be able to update joint state */
        virtual void Update() = 0;

        /** @copydoc SetBody */
        virtual Body* GetBody(JointBody body) const;

        /** Determines a body managed by the joint. One of the bodies must be movable (non-kinematic). */
        virtual void SetBody(JointBody body, Body* value);

        /** Returns the position relative to the body, at which the body is anchored to the joint. */
        virtual Vector3 GetPosition(JointBody body) const;

        /** Returns the rotation relative to the body, at which the body is anchored to the joint. */
        virtual Quaternion GetRotation(JointBody body) const;

        /** Sets the position and rotation relative to the body, at which the body is anchored to the joint.  */
        virtual void SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation);

        /** @copydoc SetBreakForce */
        virtual float GetBreakForce() const;

        /**
         * Determines the maximum force the joint can apply before breaking. Broken joints no longer participate in physics
         * simulation.
         */
        virtual void SetBreakForce(float force);

        /** @copydoc SetBreakTorque */
        virtual float GetBreakTorque() const;

        /**
         * Determines the maximum torque the joint can apply before breaking. Broken joints no longer participate in physics
         * simulation.
         */
        virtual void SetBreakTorque(float torque);

        /** @copydoc SetEnableCollision */
        virtual bool GetEnableCollision() const;

        /** Determines whether collision between the two bodies managed by the joint are enabled. */
        virtual void SetEnableCollision(bool value);

        /** Triggered when the joint's break force or torque is exceeded. */
        Event<void()> OnJointBreak;

        /** Returns the object containing common collider code. */
        FJoint* GetInternal() const { return _internal; }

        /**
         * Sets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        void SetOwner(PhysicsOwnerType type, void* owner) { _owner.Type = type; _owner.OwnerData = owner; }

        /**
         * Gets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        void* GetOwner(PhysicsOwnerType type) const { return _owner.Type == type ? _owner.OwnerData : nullptr; }

    protected:
        PhysicsObjectOwner _owner;
        FJoint* _internal = nullptr;

        BodyInfo _bodies[2];
        float _breakForce = FLT_MAX / 2;
        float _breakTorque = FLT_MAX / 2;
        bool _enableCollision = false;
    };
}
