#include "TeSphericalJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SPtr<SphericalJoint> SphericalJoint::Create(PhysicsScene& scene, const SPHERICAL_JOINT_DESC& desc)
    {
        return scene.CreateSphericalJoint(desc);
    }
}
