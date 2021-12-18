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
        static UINT32 GetComponentType() { return TypeID_Core::TID_CCollider; }

        /** @copydoc Component::Initialize */
        virtual void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HCollider& c);

        /** @copydoc Component::update */
        void Update() override { }

        /** @copydoc Collider::SetScale */
        void SetScale(const Vector3& scale);

        /** @copydoc Collider::GetScale */
        const Vector3& GetScale() const { return _scale; }

        /** @copydoc Collider::SetCenter */
        void SetCenter(const Vector3& center);

        /** @copydoc Collider::GetCenter */
        const Vector3& GetCenter() const { return _center; }

        /** @copydoc Collider::SetPosition */
        void SetPosition(const Vector3& position);

        /** @copydoc Collider::GetPosition */
        const Vector3& GetPosition() const { return _position; }

        /** @copydoc Collider::SetRotation */
        void SetRotation(const Quaternion& quaternion);

        /** @copydoc Collider::GetPosition */
        const Quaternion& GetRotation() const { return _rotation; }

        /** @copydoc Collider::GetBody */
        HRigidBody GetBody() const { return _parent; }

         /** Returns the Collider implementation wrapped by this component. */
        Collider* GetInternal() const { return _internal.get(); }

    protected:
        friend class SceneObject;
        friend class CBody;
        friend class CRigidBody;
        friend class CSoftBody;
        using Component::DestroyInternal;

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

        /** Creates the internal representation of the Joint for use by the component. */
        virtual SPtr<Collider> CreateInternal() = 0;

        /** Creates the internal representation of the Joint and restores the values saved by the Component. */
        virtual void RestoreInternal();

        /** Destroys the internal joint representation. */
        virtual void DestroyInternal();

        /**
         * Checks is the provided rigidbody is a valid parent for this collider.
         *
         * @note This is required because certain colliders are limited in how they can be used. */
        virtual bool IsValidParent(const HRigidBody& parent) const { return true; }

        /**
         * Changes the rigidbody parent of the collider. Meant to be called from the Rigidbody itself.
         *
         * @param[in] rigidbody		New rigidbody to assign as the parent to the collider.
         * @param[in] internal		If true the rigidbody will just be changed internally, but parent rigidbody will not be
         *							notified.
         */
        void SetBody(const HRigidBody& body, bool internal = false);

        /** Searches the parent scene object hierarchy to find a parent Rigidbody component. */
        void UpdateParentBody();

    protected:
        friend class CBody;
        CCollider(UINT32 type);

    protected:
        SPtr<Collider> _internal;
        HRigidBody _parent;

        Vector3 _scale = Vector3::ONE;
        Vector3 _center = Vector3::ZERO;
        Vector3 _position = Vector3::ZERO;
        Quaternion _rotation = Quaternion::IDENTITY;
    };
}
