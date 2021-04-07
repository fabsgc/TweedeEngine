#include "Components/TeCRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Components/TeCRigidBody.h"
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

            if (((UINT32)_flags & (UINT32)BodyFlag::AutoTensors) != 0)
                _internal->UpdateMassDistribution();

#if TE_DEBUG_MODE
            CheckForNestedBody();
#endif
        }

        if (gPhysics().IsUpdateInProgress())
            return;

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

         // TODO
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
            collider->SetRigidBody(HRigidBody(), true);

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

    void CRigidBody::CheckForNestedBody()
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
