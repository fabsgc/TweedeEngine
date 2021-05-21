#include "TeSphericalJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SPtr<SphericalJoint> SphericalJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateSphericalJoint();
    }
}
