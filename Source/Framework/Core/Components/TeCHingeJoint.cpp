#include "Components/TeCHingeJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CHingeJoint::CHingeJoint()
        : CJoint((UINT32)TID_CHingeJoint)
    {
        SetName("HingeJoint");
    }

    CHingeJoint::CHingeJoint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CHingeJoint)
    {
        SetName("HingeJoint");
    }

    SPtr<Joint> CHingeJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = HingeJoint::Create(*scene->GetPhysicsScene());
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    void CHingeJoint::Clone(const HHingeJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));
    }
}
