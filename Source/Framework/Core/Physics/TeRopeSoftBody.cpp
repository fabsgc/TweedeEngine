#include "Physics/TeRopeSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    RopeSoftBody::RopeSoftBody(const HSceneObject& linkedSO)
        : SoftBody(linkedSO, TypeID_Core::TID_RopeSoftBody)
    { }

    SPtr<RopeSoftBody> RopeSoftBody::Create(const HSceneObject& linkedSO)
    {
        const SPtr<SceneInstance>& scene = linkedSO->GetScene();

        if (!scene)
        {
            TE_DEBUG("Trying to create a RopeSoftBody with an uninstantiated scene object.");
            return nullptr;
        }

        return scene->GetPhysicsScene()->CreateRopeSoftBody(linkedSO);
    }
}
