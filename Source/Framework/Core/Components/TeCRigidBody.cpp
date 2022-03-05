#include "Components/TeCRigidBody.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysics.h"

#include <functional>

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

    void CRigidBody::Initialize()
    {
        ClearColliders();
        OnEnabled();
        CBody::Initialize();
    }

    bool CRigidBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CRigidBody>(c), suffix);
    }

    bool CRigidBody::Clone(const HRigidBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if(CBody::Clone(static_object_cast<CBody>(c), suffix))
        {
            _angularFactor = c->_angularFactor;
            _useGravity = c->_useGravity;
            _centerOfMass = c->_centerOfMass;

            return true;
        }

        return false;
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

    void CRigidBody::SetUseGravity(bool gravity)
    {
        if (_useGravity == gravity)
            return;

        _useGravity = gravity;

        if (_internal != nullptr)
            std::static_pointer_cast<RigidBody>(_internal)->SetUseGravity(gravity);
    }

    void CRigidBody::SetCenterOfMass(const Vector3& centerOfMass)
    {
        if (_centerOfMass == centerOfMass)
            return;

        _centerOfMass = centerOfMass;

        if (_internal != nullptr)
            std::static_pointer_cast<RigidBody>(_internal)->SetCenterOfMass(centerOfMass);
    }

    void CRigidBody::SetIsKinematic(bool kinematic)
    {
        if (_isKinematic == kinematic)
            return;

        _isKinematic = kinematic;

        if (_internal != nullptr)
        {
            _internal->SetIsKinematic(kinematic);

            ClearColliders();
            UpdateColliders();
        }
    }

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
        UpdateJoints();

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        CheckForNestedBody();
#endif

        _internalRigidBody->OnCollisionBegin.Connect(std::bind(&CRigidBody::TriggerOnCollisionBegin, this, _1));
        _internalRigidBody->OnCollisionStay.Connect(std::bind(&CRigidBody::TriggerOnCollisionStay, this, _1));
        _internalRigidBody->OnCollisionEnd.Connect(std::bind(&CRigidBody::TriggerOnCollisionEnd, this, _1));

        const Transform& tfrm = SO()->GetTransform();
        _internalRigidBody->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        _internalRigidBody->SetFriction(_friction);
        _internalRigidBody->SetRollingFriction(_rollingFriction);
        _internalRigidBody->SetRestitution(_restitution);
        _internalRigidBody->SetVelocity(_velocity);
        _internalRigidBody->SetAngularVelocity(_angularVelocity);
        _internalRigidBody->SetIsKinematic(_isKinematic);
        _internalRigidBody->SetIsDebug(_isDebug);
        _internalRigidBody->SetFlags(_flags);
        _internalRigidBody->SetCollisionReportMode(_collisionReportMode);
        _internalRigidBody->SetCollisionReportMode(_collisionReportMode);
        _internalRigidBody->SetMass(_mass);

        _internalRigidBody->SetAngularFactor(_angularFactor);
        _internalRigidBody->SetUseGravity(_useGravity);
        _internalRigidBody->SetCenterOfMass(_centerOfMass);
    }

    void CRigidBody::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & TCF_Parent) != 0)
        {
            ClearColliders();
            UpdateColliders();

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            CheckForNestedBody();
#endif
        }

        if (gPhysics().IsUpdateInProgress())
            return;

        const Transform& tfrm = SO()->GetTransform();
        _internalRigidBody->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        for (auto& joint : _joints)
        {
            joint.JointElt->NotifyBodyMoved(static_object_cast<CBody>(_thisHandle));
        }
    }

    SPtr<Body> CRigidBody::CreateInternal()
    {
        _internalRigidBody = RigidBody::Create(SO());
        _internalRigidBody->SetOwner(PhysicsOwnerType::Component, this);

        return _internalRigidBody;
    }

    void CRigidBody::DestroyInternal()
    { 
        ClearJoints();
        ClearColliders();

        if (_internal)
        {
            _internalRigidBody->SetOwner(PhysicsOwnerType::None, nullptr);
            _internal = nullptr;
        }
    }

    void CRigidBody::ClearColliders()
    {
        for (auto& collider : _colliders)
            collider->SetBody(HRigidBody(), true);

        _colliders.clear();

        if (_internal != nullptr)
            std::static_pointer_cast<RigidBody>(_internal)->RemoveColliders();
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

        _colliders.push_back(collider);

        auto iterFind = std::find(_colliders.begin(), _colliders.end(), collider);
        if (iterFind != _colliders.end())
        {
            std::static_pointer_cast<RigidBody>(_internal)->AddCollider(collider->GetInternal());
        }
    }

    void CRigidBody::RemoveCollider(const HCollider& collider)
    {
        if (_internal == nullptr)
            return;

        auto iterFind = std::find(_colliders.begin(), _colliders.end(), collider);
        if (iterFind != _colliders.end())
        {
            std::static_pointer_cast<RigidBody>(_internal)->RemoveCollider(collider->GetInternal());
            _colliders.erase(iterFind);
        }
    }

    void CRigidBody::ClearJoints()
    {
        // We keep track of _joints in case we are juste disabling the component
        // We want to put back those _joints if component is enabled again (see UpdateJoints)
        _backupJoints = _joints;
        _joints.clear();

        for (auto& joint : _backupJoints)
            joint.JointElt->SetBody(joint.JointBodyType, HRigidBody());

        if (_internal != nullptr)
            std::static_pointer_cast<RigidBody>(_internal)->RemoveJoints();
    }

    void CRigidBody::UpdateJoints()
    {
        std::static_pointer_cast<RigidBody>(_internal)->RemoveJoints();

        for (auto& joint : _backupJoints)
        {
            joint.JointElt->SetBody(joint.JointBodyType, static_object_cast<CRigidBody>(_thisHandle));
            std::static_pointer_cast<RigidBody>(_internal)->AddJoint(joint.JointElt->GetInternal());
        }
    }

    void CRigidBody::AddJoint(JointBody jointBody, const HJoint& joint)
    {
        if (_internal == nullptr)
            return;

        _joints.push_back(JointInfo(jointBody, joint));

        auto iterFind = std::find(_joints.begin(), _joints.end(), joint);
        if (iterFind != _joints.end() && joint->GetInternal())
        {
            std::static_pointer_cast<RigidBody>(_internal)->AddJoint(joint->GetInternal());
        }
    }

    void CRigidBody::RemoveJoint(JointBody jointBody, const HJoint& joint)
    {
        if (_internal == nullptr)
            return;

        auto iterFind = std::find(_joints.begin(), _joints.end(), joint);
        if (iterFind != _joints.end())
        {
            std::static_pointer_cast<RigidBody>(_internal)->RemoveJoint(joint->GetInternal());
            _joints.erase(iterFind);
        }
    }

    void CRigidBody::CheckForNestedBody()
    {
        HSceneObject currentSO = SO()->GetParent();

        while (currentSO != nullptr)
        {
            if (currentSO->HasComponent(TID_CRigidBody) || 
                currentSO->HasComponent(TID_CMeshSoftBody) ||
                currentSO->HasComponent(TID_CPatchSoftBody) ||
                currentSO->HasComponent(TID_RopeSoftBody) ||
                currentSO->HasComponent(TID_CEllipsoidSoftBody))
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
        output.ContactPoints = std::move(const_cast<CollisionDataRaw&>(raw).ContactPoints);

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
