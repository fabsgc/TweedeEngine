#include "Physics/TeRigidBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    RigidBody::RigidBody(const HSceneObject& linkedSO, UINT32 type)
        : Body(linkedSO, type)
    { }

    SPtr<RigidBody> RigidBody::Create(const HSceneObject& linkedSO)
    {
        const SPtr<SceneInstance>& scene = linkedSO->GetScene();

        if (!scene)
        {
            TE_DEBUG("Trying to create a RigidBody with an uninstantiated scene object.");
            return nullptr;
        }

        return scene->GetPhysicsScene()->CreateRigidBody(linkedSO);
    }
}
