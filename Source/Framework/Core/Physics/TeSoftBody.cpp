#include "Physics/TeSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{ 
    SoftBody::SoftBody(const HSceneObject& linkedSO)
        : Body(linkedSO, TypeID_Core::TID_SoftBody)
    { }

    SPtr<SoftBody> SoftBody::Create(const HSceneObject& linkedSO)
    {
        const SPtr<SceneInstance>& scene = linkedSO->GetScene();

        if (!scene)
        {
            TE_DEBUG("Trying to create a SoftBody with an uninstantiated scene object.");
            return nullptr;
        }

        return scene->GetPhysicsScene()->CreateSoftBody(linkedSO);
    }
}
