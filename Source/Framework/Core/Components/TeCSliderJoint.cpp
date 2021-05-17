#include "Components/TeCSliderJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CSliderJoint::CSliderJoint()
        : CJoint((UINT32)TID_CSliderJoint)
    {
        SetName("SliderJoint");
    }

    CSliderJoint::CSliderJoint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CSliderJoint)
    {
        SetName("SliderJoint");
    }

    SPtr<Joint> CSliderJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = SliderJoint::Create(*scene->GetPhysicsScene());
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    void CSliderJoint::Clone(const HSliderJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));
    }
}
