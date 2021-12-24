#include "Physics/TeMeshSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    MeshSoftBody::MeshSoftBody(const HSceneObject& linkedSO)
        : SoftBody(linkedSO) // TODO
    { }

    SPtr<MeshSoftBody> MeshSoftBody::Create(const HSceneObject& linkedSO)
    {
        const SPtr<SceneInstance>& scene = linkedSO->GetScene();

        if (!scene)
        {
            TE_DEBUG("Trying to create a MeshSoftBody with an uninstantiated scene object.");
            return nullptr;
        }

        return scene->GetPhysicsScene()->CreateMeshSoftBody(linkedSO);
    }
}
