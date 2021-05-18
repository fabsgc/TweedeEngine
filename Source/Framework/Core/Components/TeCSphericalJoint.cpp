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

        _pivots[0] = c->_pivots[0];
        _pivots[1] = c->_pivots[1];
    }

    void CSphericalJoint::SetPivot(JointBody body, const Vector3& pivot)
    {
        _pivots[(int)body] = pivot;

        if (_internal != nullptr)
            std::static_pointer_cast<SphericalJoint>(_internal)->SetPivot(body, pivot);
    }

    Vector3 CSphericalJoint::GetPivot(JointBody body)
    {
        return _pivots[(int)body];
    }

    void CSphericalJoint::OnEnabled()
    {
        CJoint::OnEnabled();

        SetPivot(JointBody::Anchor, _pivots[(int)JointBody::Anchor]);
        SetPivot(JointBody::Target, _pivots[(int)JointBody::Target]);
    }
}
