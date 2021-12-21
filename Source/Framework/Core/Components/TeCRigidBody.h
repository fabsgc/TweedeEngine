#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Physics/TeJoint.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCBody.h"
#include "Components/TeCJoint.h"
#include "TeCBoxCollider.h"
#include "TeCPlaneCollider.h"
#include "TeCSphereCollider.h"
#include "TeCCylinderCollider.h"
#include "TeCCapsuleCollider.h"
#include "TeCMeshCollider.h"
#include "TeCConeCollider.h"
#include "TeCHeightFieldCollider.h"

namespace te
{
    /** Each body needs to keep track of all joints attached to him */
    struct JointInfo
    {
        JointBody JointBodyType;
        HJoint JointElt;

        JointInfo(JointBody jointBody, const HJoint& joint)
            : JointBodyType(jointBody)
            , JointElt(joint)
        { }

        friend bool operator == (const JointInfo& lhs, const HJoint& rhs)
        {
            return lhs.JointElt == rhs;
        }
    };

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
        static UINT32 GetComponentType() { return TypeID_Core::TID_CRigidBody; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        bool Clone(const HRigidBody& c, const String& suffix = "");

        /** @copydoc Component::Update */
        void Update() override;

        /** Returns the Rigidbody implementation wrapped by this component. */
        Body* GetInternal() const override { return (RigidBody*)(_internal.get()); }

        /** @copydoc RigidBody::SetAngularFactor */
        void SetAngularFactor(const Vector3& angularFactor);

        /** @copydoc RigidBody::GetAngularFactor */
        const Vector3& GetAngularFactor() const { return _angularFactor; }

        /** @copydoc RigidBody::SetUseGravity */
        void SetUseGravity(bool gravity);

        /** @copydoc RigidBody::GetUseGravity */
        bool GetUseGravity() const { return _useGravity; }

        /** @copydoc RigidBody::SetCenterOfMass */
        void SetCenterOfMass(const Vector3& centerOfMass);

        /** @copydoc RigidBody::GetCenterOfMass */
        const Vector3& GetCenterOfMass() const { return _centerOfMass; }

        /** @copydoc Body::SetIsKinematic */
        virtual void SetIsKinematic(bool kinematic) override;

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
        friend class CHeightFieldCollider;
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

        /**
         * Searches child scene objects for Collider components and attaches them to the body. Make sure to call
         * ClearColliders() if you need to clear old colliders first.
         */
        void UpdateColliders();

        /** Unregisters all child colliders from the body. */
        void ClearColliders();

        /**
         * Registers a new collider with the body. This collider will then be used to calculate body's geometry
         * used for collisions, and optionally (depending on set flags) total mass, inertia tensors and center of mass.
         */
        void AddCollider(const HCollider& collider);

        /** Unregisters the collider from the body. */
        void RemoveCollider(const HCollider& collider);

        /** Checks if the body is nested under another body, and throws out a warning if so. */
        void CheckForNestedBody();

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
                {
                    // depending on component order in sceneGraph, collider might not be created when doing that
                    // In this case, collider will trigger the SetBody and AddCollider himself
                    component->RestoreInternal();
                }
                else
                {
                    component->SetBody(static_object_cast<CRigidBody>(_thisHandle), true);
                    AddCollider(component);
                }
            }
        }

        /** Unregisters all internal joints from the body. */
        void ClearJoints();

        /** Use _joints vector to fill internal joints list */
        void UpdateJoints();

        /** Set a joint that this body is attached to. Allows the body to notify the joint when it moves. */
        void AddJoint(JointBody jointBody, const HJoint& joint);

        /** Removes a joint that this body is attached to. */
        void RemoveJoint(JointBody jointBody, const HJoint& joint);

        /** Body::ProcessCollisionData */
        void ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output) override;

    protected:
        CRigidBody(); // Serialization only

    protected:
        SPtr<RigidBody> _internalRigidBody;

        Vector<JointInfo> _joints;
        Vector<JointInfo> _backupJoints;
        Vector<HCollider> _colliders;

        bool _useGravity = true;

        Vector3 _angularFactor = Vector3::ONE;
        Vector3 _centerOfMass = Vector3::ZERO;
    };
}
