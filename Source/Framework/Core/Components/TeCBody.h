#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Physics/TeJoint.h"
#include "Physics/TeBody.h"

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
     * @copydoc	Joint
     *
     * @note Wraps Body as a Component.
     */
    class TE_CORE_EXPORT CBody : public Component
    {
    public:
        CBody(const HSceneObject& parent, UINT32 type);
        virtual ~CBody() = default;

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CBody; }

        /** @copydoc Component::Initialize */
        virtual void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HBody& c);

        /** @copydoc Component::Update */
        void Update() override { }

        /** @copydoc Body::Move */
        void Move(const Vector3& position);

        /** @copydoc Body::Rotate */
        void Rotate(const Quaternion& rotation);

        /** @copydoc Body::SetMass */
        void SetMass(float mass);

        /** @copydoc Body::GetMass */
        float GetMass() const { return _mass; };

        /** @copydoc Body::SetIsKinematic */
        void SetIsKinematic(bool kinematic);

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

        /** @copydoc Body::SetUseGravity */
        void SetUseGravity(bool gravity);

        /** @copydoc Body::GetUseGravity */
        bool GetUseGravity() const { return _useGravity; }

        /** @copydoc Body::SetCenterOfMass */
        void SetCenterOfMass(const Vector3& centerOfMass);

        /** @copydoc Body::GetCenterOfMass */
        const Vector3& GetCenterOfMass() const { return _centerOfMass; }

        /** @copydoc Body::ApplyForce */
        void ApplyForce(const Vector3& force, ForceMode mode) const;

        /** @copydoc Body::ApplyForceAtPoint */
        void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const;

        /** @copydoc Body::ApplyTorque */
        void ApplyTorque(const Vector3& torque, ForceMode mode) const;

        /** Sets a value that determines which (if any) collision events are reported. */
        void SetCollisionReportMode(CollisionReportMode mode);

        /** Gets a value that determines which (if any) collision events are reported. */
        CollisionReportMode GetCollisionReportMode() const { return _collisionReportMode; }

        /** @copydoc Body::SetFlags */
        void SetFlags(BodyFlag flags);

        /** @copydoc Body::GetFlags */
        BodyFlag GetFlags() const { return _flags; }

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
        friend class CCollider;
        friend class CBoxCollider;
        friend class CCapsuleCollider;
        friend class CConeCollider;
        friend class CCylinderCollider;
        friend class CMeshCollider;
        friend class CPlaneCollider;
        friend class CSphereCollider;
        using Component::DestroyInternal;

        /** Triggered when the internal body begins touching another object. */
        void TriggerOnCollisionBegin(const CollisionDataRaw& data);

        /** Triggered when the internal body continues touching another object. */
        void TriggerOnCollisionStay(const CollisionDataRaw& data);

        /** Triggered when the internal body ends touching another object. */
        void TriggerOnCollisionEnd(const CollisionDataRaw& data);

        /** Destroys the internal joint representation. */
        virtual void DestroyInternal() = 0;

        /** Creates the internal representation of the Body for use by the component. */
        virtual SPtr<Body> CreateInternal() = 0;

        /**
         * Searches child scene objects for Collider components and attaches them to the body. Make sure to call
         * clearColliders() if you need to clear old colliders first.
         */
        virtual void UpdateColliders() = 0;

        /** Unregisters all child colliders from the body. */
        virtual void ClearColliders() = 0;

        /**
         * Registers a new collider with the body. This collider will then be used to calculate body's geometry
         * used for collisions, and optionally (depending on set flags) total mass, inertia tensors and center of mass.
         */
        virtual void AddCollider(const HCollider& collider) = 0;

        /** Unregisters the collider from the body. */
        virtual void RemoveCollider(const HCollider& collider) = 0;

        /** Checks if the body is nested under another body, and throws out a warning if so. */
        virtual void CheckForNestedBody() = 0;

        /** Appends Component referenes for the colliders to the collision data. */
        virtual void ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output) = 0;

        /** @copydoc Body::UpdateMassDistribution */
        virtual void UpdateMassDistribution() = 0;

    protected:
        CBody(UINT32 type);

    protected:
        SPtr<Body> _internal;
        Vector<HCollider> _children;
        HJoint _parentJoint;

        float _mass = 1.0f;
        float _friction = 0.0f;
        float _rollingFriction = 0.0f;
        float _restitution = 0.0f;
        bool _useGravity = true;
        bool _isKinematic = false;

        Vector3 _centerOfMass = Vector3::ZERO;
        Vector3 _velocity = Vector3::ZERO;
        Vector3 _angularVelocity = Vector3::ZERO;

        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::AutoTensors | (UINT32)BodyFlag::AutoMass);
        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
