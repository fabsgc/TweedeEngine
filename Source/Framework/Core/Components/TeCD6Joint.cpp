#include "Components/TeCD6Joint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CD6Joint::CD6Joint()
        : CJoint((UINT32)TID_CD6Joint)
    {
        SetName("D6Joint");
    }

    CD6Joint::CD6Joint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CD6Joint)
    {
        SetName("D6Joint");
    }

    SPtr<Joint> CD6Joint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = D6Joint::Create(*scene->GetPhysicsScene());
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    void CD6Joint::Clone(const HD6Joint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));
    }
}
