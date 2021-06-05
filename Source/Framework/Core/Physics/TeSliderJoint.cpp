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

    void SliderJoint::SetSoftnessDirLin(float softnessDirLin)
    {
        _softnessDirLin = softnessDirLin;
    }

    void SliderJoint::SetRestitutionDirLin(float restitutionDirLin)
    {
        _restitutionDirLin = restitutionDirLin;
    }

    void SliderJoint::SetDampingDirLin(float dampingDirLin)
    {
        _dampingDirLin = dampingDirLin;
    }

    void SliderJoint::SetSoftnessDirAng(float softnessDirAng)
    {
        _softnessDirAng = softnessDirAng;
    }

    void SliderJoint::SetRestitutionDirAng(float restitutionDirAng)
    {
        _restitutionDirAng = restitutionDirAng;
    }

    void SliderJoint::SetDampingDirAng(float dampingDirAng)
    {
        _dampingDirAng = dampingDirAng;
    }

    void SliderJoint::SetSoftnessLimLin(float softnessLimLin)
    {
        _softnessLimLin = softnessLimLin;
    }

    void SliderJoint::SetRestitutionLimLin(float restitutionLimLin)
    {
        _restitutionLimLin = restitutionLimLin;
    }

    void SliderJoint::SetDampingLimLin(float dampingLimLin)
    {
        _dampingLimLin = dampingLimLin;
    }
}
