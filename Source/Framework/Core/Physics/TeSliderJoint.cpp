#include "TeSliderJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SPtr<SliderJoint> SliderJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateSliderJoint();
    }
}
