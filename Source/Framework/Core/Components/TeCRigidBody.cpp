#include "Components/TeCRigidBody.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysics.h"

using namespace std::placeholders;

namespace te
{
    CRigidBody::CRigidBody()
        : CBody(HSceneObject(), (UINT32)TID_CRigidBody)
    {
        SetName("Rigidbody");
    }

    CRigidBody::CRigidBody(const HSceneObject& parent)
        : CBody(parent, (UINT32)TID_CRigidBody)
    {
        SetName("Rigidbody");
    }

    CRigidBody::~CRigidBody()
    { }

    void CRigidBody::Initialize()
    { 
        ClearColliders();
        OnEnabled();
        CBody::Initialize();
    }

    void CRigidBody::Clone(const HComponent& c)
    { 
        Clone(static_object_cast<CRigidBody>(c));
    }

    void CRigidBody::Clone(const HRigidBody& c)
    { 
        CBody::Clone(static_object_cast<CBody>(c));

        _angularFactor = c->_angularFactor;
    }

    void CRigidBody::Update()
    {
        CBody::Update();
    }

    void CRigidBody::SetAngularFactor(const Vector3& angularFactor)
    {
        if (_angularFactor == angularFactor)
            return;

        _angularFactor = angularFactor;

        if (_internal != nullptr)
            std::static_pointer_cast<RigidBody>(_internal)->SetAngularFactor(_angularFactor);
    }

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
        if (_internal != nullptr)
            DestroyInternal();

        _internal = CreateInternal();
        UpdateColliders();

#if TE_DEBUG_MODE
        CheckForNestedBody();
#endif

        _internal->OnCollisionBegin.Connect(std::bind(&CRigidBody::TriggerOnCollisionBegin, this, _1));
        _internal->OnCollisionStay.Connect(std::bind(&CRigidBody::TriggerOnCollisionStay, this, _1));
        _internal->OnCollisionEnd.Connect(std::bind(&CRigidBody::TriggerOnCollisionEnd, this, _1));

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

        std::static_pointer_cast<RigidBody>(_internal)->SetAngularFactor(_angularFactor);
    }

    void CRigidBody::OnTransformChanged(TransformChangedFlags flags)
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

        if (_parentJoint != nullptr)
            _parentJoint->NotifyBodyMoved(static_object_cast<CBody>(_thisHandle));
    }

    SPtr<Body> CRigidBody::CreateInternal()
    {
        SPtr<RigidBody> body = RigidBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }

    void CRigidBody::DestroyInternal()
    { 
        ClearColliders();

        if (_internal)
        {
            _internal->SetOwner(PhysicsOwnerType::None, nullptr);
            _internal = nullptr;
        }
    }

    void CRigidBody::ClearColliders()
    {
        for (auto& collider : _children)
            collider->SetBody(HBody(), true);

        _children.clear();

        if (_internal != nullptr)
            _internal->RemoveColliders();
    }

    void CRigidBody::UpdateColliders()
    {
        static Vector<UINT32> colliderTypes = {
            (UINT32)TID_CCollider,
            (UINT32)TID_CBoxCollider,
            (UINT32)TID_CPlaneCollider,
            (UINT32)TID_CSphereCollider,
            (UINT32)TID_CCylinderCollider,
            (UINT32)TID_CCapsuleCollider,
            (UINT32)TID_CMeshCollider,
            (UINT32)TID_CConeCollider
        };

        Stack<HSceneObject> todo;
        todo.push(SO());

        while (!todo.empty())
        {
            HSceneObject currentSO = todo.top();
            todo.pop();

            if (currentSO->HasComponent(colliderTypes))
            {
                FindAndAddColliders<CBoxCollider>(currentSO);
                FindAndAddColliders<CPlaneCollider>(currentSO);
                FindAndAddColliders<CSphereCollider>(currentSO);
                FindAndAddColliders<CCylinderCollider>(currentSO);
                FindAndAddColliders<CCapsuleCollider>(currentSO);
                FindAndAddColliders<CMeshCollider>(currentSO);
                FindAndAddColliders<CConeCollider>(currentSO);
            }

            UINT32 childCount = currentSO->GetNumChildren();
            for (UINT32 i = 0; i < childCount; i++)
            {
                HSceneObject child = currentSO->GetChild(i);

                if (child->HasComponent(TID_CRigidBody) || child->HasComponent(TID_CSoftBody))
                    continue;

                todo.push(child);
            }
        }
    }

    void CRigidBody::AddCollider(const HCollider& collider)
    {
        if (_internal == nullptr)
            return;

        _children.push_back(collider);

        auto iterFind = std::find(_children.begin(), _children.end(), collider);
        if (iterFind != _children.end())
        {
            _internal->AddCollider(collider->GetInternal());
        }
    }

    void CRigidBody::SyncCollider(const HCollider& collider)
    {
        if (_internal == nullptr)
            return;

        auto iterFind = std::find(_children.begin(), _children.end(), collider);
        if (iterFind != _children.end())
        {
            _internal->SyncCollider(collider->GetInternal());
        }
    }

    void CRigidBody::RemoveCollider(const HCollider& collider)
    {
        if (_internal == nullptr)
            return;

        auto iterFind = std::find(_children.begin(), _children.end(), collider);
        if (iterFind != _children.end())
        {
            _internal->RemoveCollider(collider->GetInternal());
            _children.erase(iterFind);
        }
    }

    void CRigidBody::CheckForNestedBody()
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

    void CRigidBody::ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output)
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
