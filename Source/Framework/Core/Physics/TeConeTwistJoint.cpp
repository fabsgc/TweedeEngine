#include "TeConeTwistJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<ConeTwistJoint> ConeTwistJoint::Create(PhysicsScene& scene, const CONE_TWIST_JOINT_DESC& desc)
    {
        return scene.CreateConeTwistJoint(desc);
    }
}
