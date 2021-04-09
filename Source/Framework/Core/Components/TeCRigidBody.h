#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneObject.h"

#include "TeCBody.h"

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

        /** @copydoc Component::update */
        void Update() override { }

        /** Returns the Rigidbody implementation wrapped by this component. */
        RigidBody* GetInternal() const { return (RigidBody*)(_internal.get()); }

    protected:
        friend class SceneObject;
        friend class CCollider;
        friend class CJoint;

        /** @copydoc Component::OnInitialized() */
        void OnInitialized() override;

        /** @copydoc Component::OnDestroyed() */
        void OnDestroyed() override;

        /** @copydoc Component::OnDisabled() */
        void OnDisabled() override;

        /** @copydoc Component::OnEnabled() */
        void OnEnabled() override;

        /** @copydoc Component::OnTransformChanged() */
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

        /** @copydoc Body::UpdateMassDistribution */
        void UpdateMassDistribution() override;

        /** Find and add colliders from the same SceneObject */
        template<class T>
        void FindAndAddColliders(HSceneObject& currentSO)
        {
            Vector<HComponent> colliders = currentSO->GetComponents<T>();
            for (auto& entry : colliders)
            {
                auto component = static_object_cast<CCollider>(entry);

                if (!component->IsValidParent(static_object_cast<CRigidBody>(_thisHandle)))
                    continue;

                Collider* collider = component->GetInternal();
                if (collider == nullptr)
                    continue;

                component->SetRigidBody(static_object_cast<CRigidBody>(_thisHandle), true);
                _children.push_back(component);

                collider->SetRigidBody((RigidBody*)_internal.get());
                _internal->AddCollider(collider);
            }
        }

    protected:
        CRigidBody(); // Serialization only
    };
}
