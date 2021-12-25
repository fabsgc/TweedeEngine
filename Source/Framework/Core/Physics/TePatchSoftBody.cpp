#include "Physics/TePatchSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    PatchSoftBody::PatchSoftBody(const HSceneObject& linkedSO)
        : SoftBody(linkedSO, TypeID_Core::TID_PatchSoftBody)
    { }

    SPtr<PatchSoftBody> PatchSoftBody::Create(const HSceneObject& linkedSO)
    {
        const SPtr<SceneInstance>& scene = linkedSO->GetScene();

        if (!scene)
        {
            TE_DEBUG("Trying to create a PatchSoftBody with an uninstantiated scene object.");
            return nullptr;
        }

        return scene->GetPhysicsScene()->CreatePatchSoftBody(linkedSO);
    }
}
