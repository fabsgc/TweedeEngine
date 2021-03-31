#include "TeSliderJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SPtr<SliderJoint> SliderJoint::Create(PhysicsScene& scene, const SLIDER_JOINT_DESC& desc)
    {
        return scene.CreateSliderJoint(desc);
    }
}
