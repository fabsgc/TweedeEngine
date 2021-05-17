#include "Components/TeCConeTwistJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CConeTwistJoint::CConeTwistJoint()
        : CJoint((UINT32)TID_CConeTwistJoint)
    {
        SetName("ConeTwistJoint");
    }

    CConeTwistJoint::CConeTwistJoint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CConeTwistJoint)
    {
        SetName("ConeTwistJoint");
    }

    SPtr<Joint> CConeTwistJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = ConeTwistJoint::Create(*scene->GetPhysicsScene());

        return joint;
    }

    void CConeTwistJoint::Clone(const HConeTwistJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));
    }
}
