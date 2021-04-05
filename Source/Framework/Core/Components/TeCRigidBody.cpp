#include "Components/TeCRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Components/TeCRigidBody.h"
#include "Components/TeCCollider.h"
#include "Physics/TePhysics.h"

using namespace std::placeholders;

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
    { 
        ClearColliders();
        OnEnabled();
        Component::Initialize();
    }

    void CRigidBody::Clone(const HComponent& c)
    { 
        Clone(static_object_cast<CRigidBody>(c));
    }

    void CRigidBody::Clone(const HRigidBody& c)
    { 
        Component::Clone(c.GetInternalPtr());

        _collisionReportMode = c->_collisionReportMode;
    }

    void CRigidBody::Move(const Vector3& position)
    {
        if (_internal != nullptr)
            _internal->Move(position);

        _notifyFlags = (TransformChangedFlags)0;
        SO()->SetWorldPosition(position);
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    void CRigidBody::Rotate(const Quaternion& rotation)
    {
        if (_internal != nullptr)
            _internal->Rotate(rotation);

        _notifyFlags = (TransformChangedFlags)0;
        SO()->SetWorldRotation(rotation);
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    void CRigidBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        if (_collisionReportMode == mode)
            return;

        _collisionReportMode = mode;

        for (auto& entry : _children)
            entry->UpdateCollisionReportMode();
    }

    void CRigidBody::UpdateMassDistribution()
    {
        if (_internal != nullptr)
            return _internal->UpdateMassDistribution();
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
        _internal = RigidBody::Create(SO());
        _internal->SetOwner(PhysicsOwnerType::Component, this);

        UpdateColliders();

#if TE_DEBUG_MODE
        CheckForNestedRigibody();
#endif

        _internal->OnCollisionBegin.Connect(std::bind(&CRigidBody::TriggerOnCollisionBegin, this, _1));
        _internal->OnCollisionStay.Connect(std::bind(&CRigidBody::TriggerOnCollisionStay, this, _1));
        _internal->OnCollisionEnd.Connect(std::bind(&CRigidBody::TriggerOnCollisionEnd, this, _1));

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        // TODO
    }

    void CRigidBody::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & TCF_Parent) != 0)
        {
            ClearColliders();
            UpdateColliders();

            // TODO

#if TE_DEBUG_MODE
            CheckForNestedRigibody();
#endif
        }

        if (gPhysics().IsUpdateInProgress())
            return;

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

         // TODO
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

    void CRigidBody::TriggerOnCollisionBegin(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionBegin(hit);
    }

    void CRigidBody::TriggerOnCollisionStay(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionStay(hit);
    }

    void CRigidBody::TriggerOnCollisionEnd(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionEnd(hit);
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

                if (child->HasComponent(TID_CRigidBody))
                    continue;

                todo.push(child);
            }
        }
    }

    void CRigidBody::ClearColliders()
    {
        for (auto& collider : _children)
            collider->SetRigidBody(HRigidBody(), true);

        _children.clear();

        if (_internal != nullptr)
            _internal->RemoveColliders();
    }

    void CRigidBody::AddCollider(const HCollider& collider)
    {
        if (_internal == nullptr)
            return;

        _children.push_back(collider);
        _internal->AddCollider(collider->GetInternal());
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

    void CRigidBody::CheckForNestedRigibody()
    {
        HSceneObject currentSO = SO()->GetParent();

        while (currentSO != nullptr)
        {
            if (currentSO->HasComponent(TID_CRigidBody))
            {
                TE_DEBUG("Nested Rigidbodies detected. This will result in inconsistent transformations. "
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

        if (raw.Colliders[0] != nullptr)
        {
            CCollider* other = (CCollider*)raw.Colliders[0]->GetOwner(PhysicsOwnerType::Component);
            output.Colliders[0] = static_object_cast<CCollider>(other->GetHandle());
        }

        if (raw.Colliders[1] != nullptr)
        {
            CCollider* other = (CCollider*)raw.Colliders[1]->GetOwner(PhysicsOwnerType::Component);
            output.Colliders[1] = static_object_cast<CCollider>(other->GetHandle());
        }
    }
}
