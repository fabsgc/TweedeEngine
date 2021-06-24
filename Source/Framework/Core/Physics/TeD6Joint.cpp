#include "TeD6Joint.h"
#include "Physics/TePhysics.h"

namespace te
{
    D6Joint::D6Joint()
        : _lowerAngLimit(0.0f)
        , _upperAngLimit(0.0f)
    { }

    SPtr<D6Joint> D6Joint::Create(PhysicsScene& scene)
    {
        return scene.CreateD6Joint();
    }

    void D6Joint::SetLowerLinLimit(float lowerLinLimit)
    {
        _lowerLinLimit = lowerLinLimit;
    }

    void D6Joint::SetUpperLinLimit(float upperLinLimit)
    {
        _upperLinLimit = upperLinLimit;
    }

    void D6Joint::SetLowerAngLimit(Degree lowerAngLimit)
    {
        _lowerAngLimit = lowerAngLimit;
    }

    void D6Joint::SetUpperAngLimit(Degree upperAngLimit)
    {
        _upperAngLimit = upperAngLimit;
    }

    void D6Joint::SetLinearSpring(bool linearSpring)
    {
        _linearSpring = linearSpring;
    }

    void D6Joint::SetAngularSpring(bool angularSpring)
    {
        _angularSpring = angularSpring;
    }

    void D6Joint::SetLinearStiffness(float linearStiffness)
    {
        _linearStiffness = linearStiffness;
    }

    void D6Joint::SetAngularStiffness(float angularStiffness)
    {
        _angularStiffness = angularStiffness;
    }

    void D6Joint::SetLinearDamping(float linearDamping)
    {
        _linearDamping = linearDamping;
    }

    void D6Joint::SetAngularDamping(float angularDamping)
    {
        _angularDamping = angularDamping;
    }
}
