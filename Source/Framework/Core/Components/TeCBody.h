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
        virtual ~CBody();

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
        bool getIsKinematic() const { return _isKinematic; }

        /** Sets a value that determines which (if any) collision events are reported. */
        void SetCollisionReportMode(CollisionReportMode mode);

        /** Gets a value that determines which (if any) collision events are reported. */
        CollisionReportMode GetCollisionReportMode() const { return _collisionReportMode; }

        /** @copydoc Body::UpdateMassDistribution */
        void UpdateMassDistribution();

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

    protected:
        friend class SceneObject;
        friend class CCollider;
        using Component::DestroyInternal;

        /** Triggered when the internal body begins touching another object. */
        void TriggerOnCollisionBegin(const CollisionDataRaw& data);

        /** Triggered when the internal body continues touching another object. */
        void TriggerOnCollisionStay(const CollisionDataRaw& data);

        /** Triggered when the internal body ends touching another object. */
        void TriggerOnCollisionEnd(const CollisionDataRaw& data);

        /** Destroys the internal joint representation. */
        virtual void DestroyInternal();

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

    protected:
        CBody(UINT32 type);

    protected:
        SPtr<Body> _internal;
        Vector<HCollider> _children;
        HJoint _parentJoint;

        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::AutoTensors | (UINT32)BodyFlag::AutoMass);
        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
        float _mass = 0.0f;
        bool _isKinematic = false;
    };
}
