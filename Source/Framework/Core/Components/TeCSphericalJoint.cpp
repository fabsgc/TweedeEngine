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

    void CSphericalJoint::Clone(const HSphericalJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));
    }

    void CSphericalJoint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        if (_offsetPivots[(int)body] == offset)
            return;

        _offsetPivots[(int)body] = offset;

        if (_internal != nullptr)
            std::static_pointer_cast<SphericalJoint>(_internal)->SetOffsetPivot(body, offset);
    }

    const Vector3& CSphericalJoint::GetOffsetPivot(JointBody body) const
    {
        return _offsetPivots[(int)body];
    }

    void CSphericalJoint::OnEnabled()
    {
        CJoint::OnEnabled();

        if (_internal)
        {
            SetOffsetPivot(JointBody::Anchor, _offsetPivots[(int)JointBody::Anchor]);
            SetOffsetPivot(JointBody::Target, _offsetPivots[(int)JointBody::Target]);
        }
    }
}
