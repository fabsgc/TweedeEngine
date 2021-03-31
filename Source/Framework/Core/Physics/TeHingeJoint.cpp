#include "TeHingeJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<HingeJoint> HingeJoint::Create(PhysicsScene& scene, const HINGE_JOINT_DESC& desc)
    {
        return scene.CreateHingeJoint(desc);
    }
}
