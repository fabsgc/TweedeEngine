#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeFJoint.h"
#include "Utility/TeEvent.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    struct JOINT_DESC;

    /**
     * Base class for all Joint types. Joints constrain how two rigidbodies move relative to one another (for example a door
     * hinge). One of the bodies in the joint must always be movable (non-kinematic).
     */
    class TE_CORE_EXPORT Joint
    {
    public:
        Joint() = default;
        virtual ~Joint() = default;

        /** @copydoc FJoint::GetBody */
        Body* GetBody(JointBody body) const;

        /** @copydoc FJoint::SetBody */
        void SetBody(JointBody body, Body* value);

        /** @copydoc FJoint::GetPosition */
        Vector3 GetPosition(JointBody body) const;

        /** @copydoc FJoint::GetRotation */
        Quaternion GetRotation(JointBody body) const;

        /** @copydoc FJoint::SetTransform */
        void SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation);

        /** @copydoc FJoint::GetBreakForce */
        float GetBreakForce() const;

        /** @copydoc FJoint::SetBreakForce */
        void SetBreakForce(float force);

        /** @copydoc FJoint::GetBreakTorque */
        float GetBreakTorque() const;

        /** @copydoc FJoint::SetBreakTorque */
        void SetBreakTorque(float torque);

        /** @copydoc FJoint::GetEnableCollision */
        bool GetEnableCollision() const;

        /** @copydoc FJoint::SetEnableCollision */
        void SetEnableCollision(bool value);

        /** Triggered when the joint's break force or torque is exceeded. */
        Event<void()> OnJointBreak;

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
    };

    /** Structure used for initializing a new Joint. */
    struct JOINT_DESC
    { 
        struct BodyInfo
        {
            Body* BodyElt = nullptr;
            Vector3 Position = Vector3::ZERO;
            Quaternion Rotation = Quaternion::IDENTITY;
        };

        BodyInfo Bodies[2];
        float BreakForce = FLT_MAX;
        float BreakTorque = FLT_MAX;
        bool EnableCollision = false;
    };
}
