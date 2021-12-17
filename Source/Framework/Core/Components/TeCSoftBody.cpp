#include "Components/TeCSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysics.h"

#include <functional>

using namespace std::placeholders;

namespace te
{
    CSoftBody::CSoftBody()
        : CBody(HSceneObject(), (UINT32)TID_CSoftBody)
    {
        SetName("SoftBody");
    }

    CSoftBody::CSoftBody(const HSceneObject& parent)
        : CBody(parent, (UINT32)TID_CSoftBody)
    {
        SetName("SoftBody");
    }

    CSoftBody::~CSoftBody()
    { }

    void CSoftBody::Initialize()
    { 
        ClearColliders();
        OnEnabled();
        CBody::Initialize();
    }

    void CSoftBody::Clone(const HComponent& c)
    { 
        Clone(static_object_cast<CSoftBody>(c));
    }

    void CSoftBody::Clone(const HSoftBody& c)
    { 
        CBody::Clone(static_object_cast<CBody>(c));
    }

    void CSoftBody::SetMesh(const HPhysicsMesh& mesh)
    {
        if (_mesh == mesh)
            return;

        _mesh = mesh;

        if (_internal != nullptr)
            std::static_pointer_cast<SoftBody>(_internal)->SetMesh(mesh);
    }

    void CSoftBody::SetScale(const Vector3& scale)
    {
        _scale = scale;

        if (_internal != nullptr)
            std::static_pointer_cast<SoftBody>(_internal)->SetScale(scale);
    }

    void CSoftBody::Update()
    {
        CBody::Update();
    }

    void CSoftBody::OnInitialized()
    { }

    void CSoftBody::OnDestroyed()
    {
        DestroyInternal();
    }

    void CSoftBody::OnDisabled()
    {
        DestroyInternal();
    }

    void CSoftBody::OnEnabled()
    {
        if (_internal != nullptr)
            DestroyInternal();

        _internal = CreateInternal();
        UpdateColliders();
        UpdateJoints();

#if TE_DEBUG_MODE
        CheckForNestedBody();
#endif

        _internal->OnCollisionBegin.Connect(std::bind(&CSoftBody::TriggerOnCollisionBegin, this, _1));
        _internal->OnCollisionStay.Connect(std::bind(&CSoftBody::TriggerOnCollisionStay, this, _1));
        _internal->OnCollisionEnd.Connect(std::bind(&CSoftBody::TriggerOnCollisionEnd, this, _1));

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        _internal->SetFriction(_friction);
        _internal->SetRollingFriction(_rollingFriction);
        _internal->SetRestitution(_restitution);
        _internal->SetVelocity(_velocity);
        _internal->SetAngularVelocity(_angularVelocity);
        _internal->SetUseGravity(_useGravity);
        _internal->SetIsKinematic(_isKinematic);
        _internal->SetIsDebug(_isDebug);
        _internal->SetFlags(_flags);
        _internal->SetCollisionReportMode(_collisionReportMode);
        _internal->SetCollisionReportMode(_collisionReportMode);
        _internal->SetCenterOfMass(_centerOfMass);
        _internal->SetMass(_mass);

        std::static_pointer_cast<SoftBody>(_internal)->SetMesh(_mesh);
        std::static_pointer_cast<SoftBody>(_internal)->SetScale(_scale);
    }

    void CSoftBody::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & TCF_Parent) != 0)
        {
            ClearColliders();
            UpdateColliders();

#if TE_DEBUG_MODE
            CheckForNestedBody();
#endif
        }

        if (gPhysics().IsUpdateInProgress())
            return;

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        for (auto& joint : _joints)
        {
            joint.JointElt->NotifyBodyMoved(static_object_cast<CBody>(_thisHandle));
        }
    }

    SPtr<Body> CSoftBody::CreateInternal()
    {
        SPtr<SoftBody> body = SoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }

    void CSoftBody::DestroyInternal()
    {
        ClearJoints();
        ClearColliders();

        if (_internal)
        {
            _internal->SetOwner(PhysicsOwnerType::None, nullptr);
            _internal = nullptr;
        }
    }

    void CSoftBody::ClearColliders()
    {
        for (auto& collider : _colliders)
            collider->SetBody(HBody(), true);

        _colliders.clear();

        if (_internal != nullptr)
            _internal->RemoveColliders();
    }

    void CSoftBody::UpdateColliders()
    {
        // TODO
    }

    void CSoftBody::AddCollider(const HCollider& collider)
    {
        // TODO
    }

    void CSoftBody::RemoveCollider(const HCollider& collider)
    {
        // TODO
    }

    void CSoftBody::ClearJoints()
    {
        // We keep track of _joints in case we are juste disabling the component
        // We want to put back those _joints if component is enabled again (see UpdateJoints)
        _backupJoints = _joints;
        _joints.clear();

        for (auto& joint : _backupJoints)
            joint.JointElt->SetBody(joint.JointBodyType, HBody());

        if (_internal != nullptr)
            _internal->RemoveJoints();
    }

    void CSoftBody::UpdateJoints()
    {
        // TODO
    }

    void CSoftBody::AddJoint(JointBody jointBody, const HJoint& joint)
    {
        // TODO
    }

    void CSoftBody::RemoveJoint(JointBody jointBody, const HJoint& joint)
    {
        // TODO
    }

    void CSoftBody::CheckForNestedBody()
    {
        HSceneObject currentSO = SO()->GetParent();

        while (currentSO != nullptr)
        {
            if (currentSO->HasComponent(TID_CRigidBody) || currentSO->HasComponent(TID_CSoftBody))
            {
                TE_DEBUG("Nested Rigidbodies or SoftBodies detected. This will result in inconsistent transformations. "
                    "To parent one Rigidbody to another move its colliders to the new parent, but remove the Rigidbody "
                    "component.");
                return;
            }

            currentSO = currentSO->GetParent();
        }
    }

    void CSoftBody::ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output)
    {
        output.ContactPoints = std::move(raw.ContactPoints);

        if (raw.Bodies[0] != nullptr)
        {
            CBody* other = (CBody*)raw.Bodies[0]->GetOwner(PhysicsOwnerType::Component);
            output.Bodies[0] = static_object_cast<CBody>(other->GetHandle());
        }

        if (raw.Bodies[1] != nullptr)
        {
            CBody* other = (CBody*)raw.Bodies[1]->GetOwner(PhysicsOwnerType::Component);
            output.Bodies[1] = static_object_cast<CBody>(other->GetHandle());
        }
    }
}
