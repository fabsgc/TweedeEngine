#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Physics/TeBody.h"

namespace te
{
    /**
     * @copydoc	Body
     *
     * @note Wraps Body as a Component.
     */
    class TE_CORE_EXPORT CBody : public Component
    {
    public:
        CBody(const HSceneObject& parent, UINT32 type);
        virtual ~CBody() = 0;

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CBody; }

        /** @copydoc Component::Initialize */
        virtual void Initialize() override;

        /** @copydoc Component::Update */
        void Update() override;

        /** Returns the current bounding box */
        AABox GetBoundingBox();

        /** @copydoc Body::SetIsTrigger */
        void SetIsTrigger(bool value);

        /** @copydoc Body::GetIsTrigger */
        bool GetIsTrigger() const { return _isTrigger; }

        /** @copydoc Body::SetIsDebug */
        void SetIsDebug(bool debug);

        /** @copydoc Body::GetIsDebug */
        bool GetIsDebug() const { return _isDebug; }

        /** @copydoc Body::SetMass */
        void SetMass(float mass);

        /** @copydoc Body::GetMass */
        float GetMass() const { return _mass; };

        /** @copydoc Body::SetIsKinematic */
        virtual void SetIsKinematic(bool kinematic);

        /** @copydoc Body::GetIsKinematic */
        bool GetIsKinematic() const { return _isKinematic; }

        /** @copydoc Body::SetVelocity */
        void SetVelocity(const Vector3& velocity);

        /** @copydoc Body::GetVelocity */
        const Vector3& GetVelocity() const { return _velocity; }

        /** @copydoc Body::SetAngularVelocity */
        void SetAngularVelocity(const Vector3& velocity);

        /** @copydoc Body::GetAngularVelocity */
        const Vector3& GetAngularVelocity() const { return _angularVelocity; }

        /** @copydoc Body::SetFriction */
        void SetFriction(float friction);

        /** @copydoc Body::GetFriction */
        float GetFriction() const { return _friction; }

        /** @copydoc Body::SetRollingFriction */
        void SetRollingFriction(float rollingFriction);

        /** @copydoc Body::GetRollingFriction */
        float GetRollingFriction() const { return _rollingFriction; }

        /** @copydoc Body::SetRestitution */
        void SetRestitution(float restitution);

        /** @copydoc Body::GetRestitution */
        float GetRestitution() const { return _restitution; }

        /** @copydoc Body::ApplyForce */
        void ApplyForce(const Vector3& force, ForceMode mode) const;

        /** @copydoc Body::ApplyForceAtPoint */
        void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const;

        /** @copydoc Body::ApplyTorque */
        void ApplyTorque(const Vector3& torque, ForceMode mode) const;

        /** @copydoc Body::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode);

        /** @copydoc Body::GetCollisionReportMode */
        CollisionReportMode GetCollisionReportMode() const { return _collisionReportMode; }

        /** @copydoc Body::SetFlags */
        void SetFlags(BodyFlag flags);

        /** @copydoc Body::GetFlags */
        BodyFlag GetFlags() const { return _flags; }

        /** @copy Body::SetLinkedSO */
        void SetLinkedSO(const HSceneObject& SO);

        /** @copydoc Body::OnCollisionBegin */
        Event<void(const CollisionData&)> OnCollisionBegin;

        /** @copydoc Body::OnCollisionStay */
        Event<void(const CollisionData&)> OnCollisionStay;

        /** @copydoc Body::OnCollisionEnd */
        Event<void(const CollisionData&)> OnCollisionEnd;

        /** Returns the body implementation wrapped by this component. */
        virtual Body* GetInternal() const = 0;

    protected:
        friend class SceneObject;
        using Component::DestroyInternal;

        /** @copydoc Component::Clone */
        bool Clone(const HBody& c, const String& suffix = "");

        /** As debug is heavy for GPU, we can cull debug info for non visible bodies */
        void CullDebug(bool cull);

        /** Triggered when the internal body begins touching another object. */
        void TriggerOnCollisionBegin(const CollisionDataRaw& data);

        /** Triggered when the internal body continues touching another object. */
        void TriggerOnCollisionStay(const CollisionDataRaw& data);

        /** Triggered when the internal body ends touching another object. */
        void TriggerOnCollisionEnd(const CollisionDataRaw& data);

        /** Destroys the internal body representation. */
        virtual void DestroyInternal() = 0;

        /** Creates the internal representation of the Body for use by the component. */
        virtual SPtr<Body> CreateInternal() = 0;

        /** Appends Component referenes for the colliders to the collision data. */
        virtual void ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output) = 0;

    protected:
        CBody(UINT32 type);

    protected:
        SPtr<Body> _internal;

        float _mass = 1.0f;
        float _friction = 0.0f;
        float _rollingFriction = 0.0f;
        float _restitution = 0.0f;
        bool _isKinematic = false;
        bool _isTrigger = false;
        bool _isDebug = true;

        Vector3 _velocity = Vector3::ZERO;
        Vector3 _angularVelocity = Vector3::ZERO;

        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::None);
        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
