#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Physics/TeJoint.h"

namespace te
{
    /**
     * @copydoc	Joint
     *
     * @note Wraps Joint as a Component.
     */
    class TE_CORE_EXPORT CJoint : public Component
    {
    public:
        CJoint(const HSceneObject& parent, UINT32 type);
        virtual ~CJoint();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CJoint; }

        /** @copydoc Component::Initialize */
        virtual void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HJoint& c);

        /** @copydoc Component::Update */
        void Update() override;

        /** @copydoc Joint::getBody */
        HRigidBody GetBody(JointBody body) const;

        /** @copydoc Joint::setBody */
        void SetBody(JointBody body, const HRigidBody& value);

        /** @copydoc Joint::getPosition */
        Vector3 GetPosition(JointBody body) const;

        /** @copydoc Joint::getRotation */
        Quaternion GetRotation(JointBody body) const;

        /** @copydoc Joint::setTransform */
        void SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation);

        /** @copydoc Joint::getBreakForce */
        float GetBreakForce() const;

        /** @copydoc Joint::setBreakForce */
        void SetBreakForce(float force);

        /** @copydoc Joint::getBreakTorque */
        float GetBreakTorque() const;

        /** @copydoc Joint::setBreakTorque */
        void SetBreakTorque(float torque);

        /** @copydoc Joint::getEnableCollision */
        bool GetEnableCollision() const;

        /** @copydoc Joint::setEnableCollision */
        void SetEnableCollision(bool value);

        /* @copydoc SphericalJoint::SetOffsetPivot */
        void SetOffsetPivot(JointBody body, const Vector3& offset);

        /** @copydoc SphericalJoint::GetOffsetPivot */
        const Vector3& GetOffsetPivot(JointBody body) const;

        /** @copydoc Joint::OnJointBreak */
        Event<void()> OnJointBreak;

         /** Returns the Joint implementation wrapped by this component. */
        Joint* GetInternal() const { return _internal.get(); }

    protected:
        friend class SceneObject;
        friend class CRigidBody;
        friend class CSoftBody;
        friend class CBody;
        using Component::DestroyInternal;

        /** @copydoc Component::OnInitialized */
        virtual void OnInitialized() override;

        /** @copydoc Component::OnDestroyed */
        virtual void OnDestroyed() override;

        /** @copydoc Component::OnDisabled */
        virtual void OnDisabled() override;

        /** @copydoc Component::OnEnabled */
        virtual void OnEnabled() override;

        /** @copydoc Component::OnTransformChanged */
        virtual void OnTransformChanged(TransformChangedFlags flags) override;

        /** Creates the internal representation of the Joint for use by the component. */
        virtual SPtr<Joint> CreateInternal() = 0;

        /** Creates the internal representation of the Joint and restores the values saved by the Component. */
        virtual void RestoreInternal();

        /** Destroys the internal joint representation. */
        virtual void DestroyInternal();

        /** Calculates the local position/rotation that needs to be applied to the particular joint body. */
        virtual void GetLocalTransform(JointBody body, Vector3& position, Quaternion& rotation);

        /** Notifies the joint that one of the attached body moved and that its transform needs updating. */
        void NotifyBodyMoved(const HBody& body);

        /** Checks can the provided rigidbody be used for initializing the joint. */
        bool IsBodyValid(const HRigidBody& body);

        /** Updates the local transform for the specified body attached to the joint. */
        void UpdateTransform(JointBody body);

        /** Triggered when the joint constraint gets broken. */
        void TriggerOnJointBroken();

    protected:
        CJoint(UINT32 type);

    protected:
        SPtr<Joint> _internal;

        HRigidBody _bodies[2];
        Vector3 _positions[2];
        Quaternion _rotations[2];

        float _breakForce = 32768;
        float _breakTorque = 32768;
        bool _enableCollision = false;
        Vector3 _offsetPivots[2] = { Vector3::ZERO, Vector3::ZERO };
    };
}
