#include "TeSphericalJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SPtr<SphericalJoint> SphericalJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateSphericalJoint();
    }

    void SphericalJoint::SetPivot(JointBody body, const Vector3& pivot)
    {
        _pivots[(int)body] = pivot;
    }

    Vector3 SphericalJoint::GetPivot(JointBody body)
    {
        return _pivots[(int)body];
    }
}
