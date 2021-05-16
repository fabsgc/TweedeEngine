#include "Components/TeCSphericalJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CSphericalJoint::CSphericalJoint()
        : CJoint(_desc, (UINT32)TID_CSphericalJoint)
    {
        SetName("SphericalJoint");
    }

    CSphericalJoint::CSphericalJoint(const HSceneObject& parent)
        : CJoint(parent, _desc, (UINT32)TID_CSphericalJoint)
    {
        SetName("SphericalJoint");
    }

    SPtr<Joint> CSphericalJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = SphericalJoint::Create(*scene->GetPhysicsScene(), _desc);
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    void CSphericalJoint::Clone(const HSphericalJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));

        _desc = c->_desc;
    }
}
