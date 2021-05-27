#include "TeSliderJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SliderJoint::SliderJoint()
        : _lowerAngLimit(0.0f)
        , _upperAngLimit(0.0f)
    { }

    SPtr<SliderJoint> SliderJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateSliderJoint();
    }

    void SliderJoint::SetLowerLinLimit(float lowerLinLimit)
    {
        _lowerLinLimit = lowerLinLimit;
    }

    void SliderJoint::SetUpperLinLimit(float upperLinLimit)
    {
        _upperLinLimit = upperLinLimit;
    }

    void SliderJoint::SetLowerAngLimit(Degree lowerAngLimit)
    {
        _lowerAngLimit = lowerAngLimit;
    }

    void SliderJoint::SetUpperAngLimit(Degree upperAngLimit)
    {
        _upperAngLimit = upperAngLimit;
    }
}
