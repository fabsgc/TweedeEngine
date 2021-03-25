#include "Physics/TePhysics.h"

namespace te
{
    Physics::Physics(const PHYSICS_INIT_DESC& init)
    { }

    Physics& gPhysics()
    {
        return Physics::Instance();
    }
}
