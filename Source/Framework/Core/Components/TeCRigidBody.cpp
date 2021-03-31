#include "Components/TeCRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    CRigidBody::CRigidBody()
        : Component(HSceneObject(), (UINT32)TID_CRigidBody)
    {
        SetName("Rigidbody");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CRigidBody::CRigidBody(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CRigidBody)
    {
        SetName("Rigidbody");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CRigidBody::~CRigidBody()
    { }

    void CRigidBody::Initialize()
    { }

    void CRigidBody::Clone(const HComponent& c)
    { }

    void CRigidBody::Clone(const HRigidBody& c)
    { }

    void CRigidBody::OnInitialized()
    { }

    void CRigidBody::OnDestroyed()
    {
        DestroyInternal();
    }

    void CRigidBody::OnDisabled()
    {
        DestroyInternal();
    }

    void CRigidBody::OnEnabled()
    {
        _internal = RigidBody::Create(SO());
    }

    void CRigidBody::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & TCF_Parent) != 0)
        {

        }

        if (gPhysics().IsUpdateInProgress())
            return;
    }

    void CRigidBody::DestroyInternal()
    { 
        if (_internal)
        {
            _internal = nullptr;
        }
    }
}