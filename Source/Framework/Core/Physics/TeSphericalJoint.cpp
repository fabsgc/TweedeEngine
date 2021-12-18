#include "TeSphericalJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SphericalJoint::SphericalJoint()
        : Joint(TypeID_Core::TID_SphericalJoint)
    { }

    SPtr<SphericalJoint> SphericalJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateSphericalJoint();
    }
}
