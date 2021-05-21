#include "TeSphericalJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SPtr<SphericalJoint> SphericalJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateSphericalJoint();
    }

    void SphericalJoint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        _offsetPivots[(int)body] = offset;
    }

    const Vector3& SphericalJoint::GetOffsetPivot(JointBody body) const
    {
        return _offsetPivots[(int)body];
    }
}
