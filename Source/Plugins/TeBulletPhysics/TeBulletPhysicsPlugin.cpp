#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsManager.h"
#include "TeBulletPhysics.h"

namespace te
{
    class TE_PLUGIN_EXPORT BulletPhysicsFactory : public PhysicsFactory
    {
    public:
        void StartUp() override
        {
            PHYSICS_INIT_DESC desc;
            Physics::StartUp<BulletPhysics>(desc);
        }

        void ShutDown() override
        {
            Physics::ShutDown();
        }
    };

    extern "C" TE_PLUGIN_EXPORT BulletPhysicsFactory* LoadPlugin()
    {
        return te_new<BulletPhysicsFactory>();
    }

    extern "C" TE_PLUGIN_EXPORT void UnloadPlugin(PhysicsFactory* instance)
    {
        te_delete(instance);
    }
}
