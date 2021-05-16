#include "TeJoint.h"

namespace te
{ 
    Body* Joint::GetBody(JointBody body) const
    {
        return _bodies[(int)body].BodyElt;
    }

    void Joint::SetBody(JointBody body, Body* value)
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

    void Joint::SetEnableCollision(bool value)
    {
        _enableCollision = value;
    }
}
