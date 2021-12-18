#include "TeJoint.h"
#include "Physics/TeRigidBody.h"

namespace te
{
    Joint::Joint(UINT32 type)
        : Serializable(type)
    { }

    RigidBody* Joint::GetBody(JointBody body) const
    {
        return _bodies[(int)body].BodyElt;
    }

    void Joint::SetBody(JointBody body, RigidBody* value)
    {
        _bodies[(int)body].BodyElt = value;
    }

    Vector3 Joint::GetPosition(JointBody body) const
    {
        return _bodies[(int)body].Position;
    }

    Quaternion Joint::GetRotation(JointBody body) const
    {
        return _bodies[(int)body].Rotation;
    }

    void Joint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        _bodies[(int)body].Position = position;
        _bodies[(int)body].Rotation = rotation;
    }

    float Joint::GetBreakForce() const
    {
        return _breakForce;
    }

    void Joint::SetBreakForce(float force)
    {
        _breakForce = force;
    }

    float Joint::GetBreakTorque() const
    {
        return _breakTorque;
    }

    void Joint::SetBreakTorque(float torque)
    {
        _breakTorque = torque;
    }

    bool Joint::GetEnableCollision() const
    {
        return _enableCollision;
    }

    void Joint::SetEnableCollision(bool collision)
    {
        _enableCollision = collision;
    }

    void Joint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        _offsetPivots[(int)body] = offset;
    }

    const Vector3& Joint::GetOffsetPivot(JointBody body) const
    {
        return _offsetPivots[(int)body];
    }

    bool Joint::GetIsBroken()
    {
        return _isBroken;
    }

    void Joint::SetIsBroken(bool isBroken)
    {
        _isBroken = isBroken;
    }
}
