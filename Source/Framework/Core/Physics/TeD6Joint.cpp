#include "TeD6Joint.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<D6Joint> D6Joint::Create(PhysicsScene& scene, const D6_JOINT_DESC& desc)
    {
        return scene.CreateD6Joint(desc);
    }
}
