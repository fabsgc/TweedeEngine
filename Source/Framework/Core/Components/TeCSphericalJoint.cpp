#include "Components/TeCSphericalJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CSphericalJoint::CSphericalJoint()
        : CJoint((UINT32)TID_CSphericalJoint)
    {
        SetName("SphericalJoint");
    }

    CSphericalJoint::CSphericalJoint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CSphericalJoint)
    {
        SetName("SphericalJoint");
    }

    SPtr<Joint> CSphericalJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = SphericalJoint::Create(*scene->GetPhysicsScene());
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    bool CSphericalJoint::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CSphericalJoint>(c), suffix);
    }

    bool CSphericalJoint::Clone(const HSphericalJoint& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CJoint::Clone(static_object_cast<CJoint>(c), suffix))
            return true;

        return false;
    }

    void CSphericalJoint::OnEnabled()
    {
        CJoint::OnEnabled();
    }
}
