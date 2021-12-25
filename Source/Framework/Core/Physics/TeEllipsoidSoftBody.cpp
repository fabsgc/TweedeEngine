#include "Physics/TeEllipsoidSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    EllipsoidSoftBody::EllipsoidSoftBody(const HSceneObject& linkedSO)
        : SoftBody(linkedSO, TypeID_Core::TID_EllipsoidSoftBody)
    { }

    SPtr<EllipsoidSoftBody> EllipsoidSoftBody::Create(const HSceneObject& linkedSO)
    {
        const SPtr<SceneInstance>& scene = linkedSO->GetScene();

        if (!scene)
        {
            TE_DEBUG("Trying to create a EllipsoidSoftBody with an uninstantiated scene object.");
            return nullptr;
        }

        return scene->GetPhysicsScene()->CreateEllipsoidSoftBody(linkedSO);
    }
}
