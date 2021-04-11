#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Physics/TeCollider.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /**
     * @copydoc	Collider
     *
     * @note Wraps Collider as a Component.
     */
    class TE_CORE_EXPORT CCollider : public Component
    {
    public:
        CCollider(const HSceneObject& parent, UINT32 type);
        virtual ~CCollider();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CCollider; }

        /** @copydoc Component::Initialize */
        virtual void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HCollider& c);

        /** @copydoc Component::update */
        void Update() override { }

        /** @copydoc Collider::GetIsTrigger */
        void SetIsTrigger(bool value);

        /** @copydoc Collider::GetIsTrigger */
        bool GetIsTrigger() const { return _isTrigger; }

        /** @copydoc Collider::SetScale */
        void SetScale(const Vector3& scale);

        /** @copydoc Collider::GetScale */
        const Vector3& GetScale() const { return _scale; }

        /** @copydoc Collider::SetMass */
        void SetMass(float mass);

        /** @copydoc Collider::GetMass */
        float GetMass() const { return _mass; }

        /** @copydoc Collider::SetCenter */
        void SetCenter(const Vector3& center);

        /** @copydoc Collider::GetCenter */
        const Vector3& GetCenter() const { return _center; }

        /** @copydoc Collider::GetBody */
        HBody GetBody() const { return _parent; }

        /** @copydoc Collider::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode);

        /** @copydoc Collider::GetCollisionReportMode */
        CollisionReportMode GetCollisionReportMode() const { return _collisionReportMode; }

        /** @copydoc Collider::onCollisionBegin */
        Event<void(const CollisionData&)> OnCollisionBegin;

        /** @copydoc Collider::onCollisionStay */
        Event<void(const CollisionData&)> OnCollisionStay;

        /** @copydoc Collider::onCollisionEnd */
        Event<void(const CollisionData&)> OnCollisionEnd;

         /** Returns the Collider implementation wrapped by this component. */
        Collider* GetInternal() const { return _internal.get(); }

    protected:
        friend class SceneObject;
        friend class CBody;
        friend class CRigidBody;
        friend class CSoftBody;
        using Component::DestroyInternal;

        /** @copydoc Component::onInitialized() */
        void OnInitialized() override;

        /** @copydoc Component::onDestroyed() */
        void OnDestroyed() override;

        /** @copydoc Component::onDisabled() */
        void OnDisabled() override;

        /** @copydoc Component::onEnabled() */
        void OnEnabled() override;

        /** @copydoc Component::onTransformChanged() */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** Creates the internal representation of the Joint for use by the component. */
        virtual SPtr<Collider> CreateInternal() = 0;

        /** Creates the internal representation of the Joint and restores the values saved by the Component. */
        virtual void RestoreInternal();

        /** Destroys the internal joint representation. */
        virtual void DestroyInternal();

        /**
         * Checks is the provided rigidbody a valid parent for this collider.
         *
         * @note This is required because certain colliders are limited in how they can be used. */
        virtual bool IsValidParent(const HBody& parent) const { return true; }

        /**
         * Changes the rigidbody parent of the collider. Meant to be called from the Rigidbody itself.
         *
         * @param[in] rigidbody		New rigidbody to assign as the parent to the collider.
         * @param[in] internal		If true the rigidbody will just be changed internally, but parent rigidbody will not be
         *							notified.
         */
        void SetBody(const HBody& rigidbody, bool internal = false);

        /**
         * Updates the transform of the internal Collider representation from the transform of the component's scene object.
         */
        void UpdateTransform();

        /** Applies the collision report mode to the internal collider depending on the current state. */
        void UpdateCollisionReportMode();

        /** Searches the parent scene object hierarchy to find a parent Rigidbody component. */
        void UpdateParentBody();

        /** Triggered when the internal collider begins touching another object. */
        void TriggerOnCollisionBegin(const CollisionDataRaw& data);

        /** Triggered when the internal collider continues touching another object. */
        void TriggerOnCollisionStay(const CollisionDataRaw& data);

        /** Triggered when the internal collider ends touching another object. */
        void TriggerOnCollisionEnd(const CollisionDataRaw& data);

    protected:
        friend class CBody;
        CCollider(UINT32 type);

    protected:
        SPtr<Collider> _internal;
        HBody _parent;

        float _mass = 1.0f;
        bool _isTrigger = false;
        Vector3 _scale = Vector3::ONE;
        Vector3 _center = Vector3::ZERO;
        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
