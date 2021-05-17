#include "TeHingeJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<HingeJoint> HingeJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateHingeJoint();
    }
}
