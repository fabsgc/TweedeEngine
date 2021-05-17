#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCBody.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	Rigidbody
     *
     * @note Wraps RigidBody as a Component.
     */
    class TE_CORE_EXPORT CRigidBody : public CBody
    {
    public:
        CRigidBody(const HSceneObject& parent);
        ~CRigidBody();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CRigidBody; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HRigidBody& c);

        /** @copydoc Component::Update */
        void Update() override;

        /** Returns the Rigidbody implementation wrapped by this component. */
        Body* GetInternal() const override { return (RigidBody*)(_internal.get()); }

        /** @copydoc RigidBody::SetAngularFactor */
        void SetAngularFactor(const Vector3& angularFactor);

        /** @copydoc RigidBody::GetAngularFactor */
        const Vector3& GetAngularFactor() const { return _angularFactor; }

    protected:
        friend class SceneObject;
        friend class CCollider;
        friend class CJoint;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

        /** @copydoc Component::OnDisabled */
        void OnDisabled() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** Destroys the internal RigidBody representation. */
        void DestroyInternal() override;

        /** Body::UpdateColliders */
        void UpdateColliders() override;

        /** Body::ClearColliders */
        void ClearColliders() override;

        /** Body::AddCollider */
        void AddCollider(const HCollider& collider) override;

        /** Body::RemoveCollider */
        void RemoveCollider(const HCollider& collider) override;

        /** Body::CheckForNestedBody */
        void CheckForNestedBody() override;

        /** Body::ProcessCollisionData */
        void ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output) override;

        /** Find and add colliders from the same SceneObject */
        template<class T>
        void FindAndAddColliders(HSceneObject& currentSO)
        {
            Vector<HComponent> colliders = currentSO->GetComponents<T>();
            for (auto& entry : colliders)
            {
                auto component = static_object_cast<CCollider>(entry);

                if (!component->IsValidParent(static_object_cast<CBody>(_thisHandle)))
                    continue;

                Collider* collider = component->GetInternal();
                if (collider == nullptr)
                {
                    // depending on component order in sceneGraph, collider might not be created when doing that
                    // In this case, collider will trigger the SetBody and AddCollider himself
                    component->RestoreInternal();
                }
                else
                {
                    component->SetBody(static_object_cast<CBody>(_thisHandle), true);
                    AddCollider(component);
                }
            }
        }

        /** @copydoc CBody::ClearJoints */
        void ClearJoints() override;

        /** @copydoc CBody::UpdateJoints */
        void UpdateJoints() override;

        /** @copydoc CBody::AddJoint */
        void AddJoint(const HJoint& joint) override;

        /** @copydoc CBody::RemoveJoint */
        void RemoveJoint(const HJoint& joint) override;

    protected:
        CRigidBody(); // Serialization only

    protected:
        Vector3 _angularFactor = Vector3::ONE;
    };
}
