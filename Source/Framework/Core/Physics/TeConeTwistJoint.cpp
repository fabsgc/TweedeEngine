#include "TeConeTwistJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<ConeTwistJoint> ConeTwistJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateConeTwistJoint();
    }
}
