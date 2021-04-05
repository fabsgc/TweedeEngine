#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Scene/TeComponent.h"

#include "TeCBoxCollider.h"
#include "TeCPlaneCollider.h"
#include "TeCSphereCollider.h"
#include "TeCCylinderCollider.h"
#include "TeCCapsuleCollider.h"
#include "TeCMeshCollider.h"
#include "TeCConeCollider.h"

namespace te
{
    /**
     * @copydoc	Rigidbody
     *
     * @note Wraps RigidBody as a Component.
     */
    class TE_CORE_EXPORT CRigidBody : public Component
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

        /** @copydoc Rigidbody::Move */
        void Move(const Vector3& position);

        /** @copydoc Rigidbody::Rotate */
        void Rotate(const Quaternion& rotation);

        /** Sets a value that determines which (if any) collision events are reported. */
        void SetCollisionReportMode(CollisionReportMode mode);

        /** Gets a value that determines which (if any) collision events are reported. */
        CollisionReportMode GetCollisionReportMode() const { return _collisionReportMode; }

        /** @copydoc Rigidbody::OnCollisionBegin */
        Event<void(const CollisionData&)> OnCollisionBegin;

        /** @copydoc Rigidbody::OnCollisionStay */
        Event<void(const CollisionData&)> OnCollisionStay;

        /** @copydoc Rigidbody::OnCollisionEnd */
        Event<void(const CollisionData&)> OnCollisionEnd;

        /** Returns the Rigidbody implementation wrapped by this component. */
        RigidBody* GetInternal() const { return _internal.get(); }

        /** @copydoc Rigidbody::updateMassDistribution */
        void UpdateMassDistribution();

    protected:
        friend class SceneObject;
        friend class CCollider;
        using Component::DestroyInternal;

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

        /** Destroys the internal RigidBody representation. */
        virtual void DestroyInternal();

        /** Triggered when the internal rigidbody begins touching another object. */
        void TriggerOnCollisionBegin(const CollisionDataRaw& data);

        /** Triggered when the internal rigidbody continues touching another object. */
        void TriggerOnCollisionStay(const CollisionDataRaw& data);

        /** Triggered when the internal rigidbody ends touching another object. */
        void TriggerOnCollisionEnd(const CollisionDataRaw& data);

        /**
         * Searches child scene objects for Collider components and attaches them to the rigidbody. Make sure to call
         * clearColliders() if you need to clear old colliders first.
         */
        void UpdateColliders();

        /** Unregisters all child colliders from the Rigidbody. */
        void ClearColliders();

        /**
         * Registers a new collider with the Rigidbody. This collider will then be used to calculate Rigidbody's geometry
         * used for collisions, and optionally (depending on set flags) total mass, inertia tensors and center of mass.
         */
        void AddCollider(const HCollider& collider);

        /** Unregisters the collider from the Rigidbody. */
        void RemoveCollider(const HCollider& collider);

        /** Checks if the rigidbody is nested under another rigidbody, and throws out a warning if so. */
        void CheckForNestedRigibody();

        /** Appends Component referenes for the colliders to the collision data. */
        void ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output);

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

                collider->SetRigidBody(_internal.get());
                _internal->AddCollider(collider);
            }
        }

    protected:
        CRigidBody(); // Serialization only

    protected:
        SPtr<RigidBody> _internal;
        Vector<HCollider> _children;
        HJoint _parentJoint;

        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
