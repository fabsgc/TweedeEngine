#include "Components/TeCConeTwistJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CConeTwistJoint::CConeTwistJoint()
        : CJoint(_desc, (UINT32)TID_CConeTwistJoint)
    {
        SetName("ConeTwistJoint");
    }

    CConeTwistJoint::CConeTwistJoint(const HSceneObject& parent)
        : CJoint(parent, _desc, (UINT32)TID_CConeTwistJoint)
    {
        SetName("ConeTwistJoint");
    }

    SPtr<Joint> CConeTwistJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = ConeTwistJoint::Create(*scene->GetPhysicsScene(), _desc);

        return joint;
    }

    void CConeTwistJoint::Clone(const HConeTwistJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));
    }
}
