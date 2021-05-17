#include "TeD6Joint.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<D6Joint> D6Joint::Create(PhysicsScene& scene)
    {
        return scene.CreateD6Joint();
    }
}
