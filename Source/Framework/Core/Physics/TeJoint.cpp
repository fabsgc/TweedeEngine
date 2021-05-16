#include "TeJoint.h"

namespace te
{ 
    Body* Joint::GetBody(JointBody body) const
    {
        return _internal->GetBody(body);
    }

    void Joint::SetBody(JointBody body, Body* value)
    {
        _internal->SetBody(body, value);
    }

    Vector3 Joint::GetPosition(JointBody body) const
    {
        return _internal->GetPosition(body);
    }

    Quaternion Joint::GetRotation(JointBody body) const
    {
        return _internal->GetRotation(body);
    }

    void Joint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        _internal->SetTransform(body, position, rotation);
    }

    float Joint::GetBreakForce() const
    {
        return _internal->GetBreakForce();
    }

    void Joint::SetBreakForce(float force)
    {
        _internal->SetBreakForce(force);
    }

    float Joint::GetBreakTorque() const
    {
        return _internal->GetBreakTorque();
    }

    void Joint::SetBreakTorque(float torque)
    {
        _internal->SetBreakTorque(torque);
    }

    bool Joint::GetEnableCollision() const
    {
        return _internal->GetEnableCollision();
    }

    void Joint::SetEnableCollision(bool value)
    {
        _internal->SetEnableCollision(value);
    }
}
