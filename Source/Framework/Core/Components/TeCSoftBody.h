#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSoftBody.h"
#include "Components/TeCBody.h"

namespace te
{
    /**
     * @copydoc	SoftBody
     *
     * @note Wraps SoftBody as a Component.
     */
    class TE_CORE_EXPORT CSoftBody : public CBody
    {
    public:
        CSoftBody(const HSceneObject& parent);
        ~CSoftBody();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CSoftBody; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HSoftBody& c);

        /** @copydoc Component::update */
        void Update() override { }

        /** Returns the SoftBody implementation wrapped by this component. */
        SoftBody* GetInternal() const { return (SoftBody*)_internal.get(); }

    protected:
        friend class SceneObject;
        friend class CCollider;

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

        /** Destroys the internal SoftBody representation. */
        virtual void DestroyInternal() override;

        /** Body::UpdateColliders */
        virtual void UpdateColliders() override;

        /** Body::ClearColliders */
        virtual void ClearColliders() override;

        /** Body::AddCollider */
        virtual void AddCollider(const HCollider& collider) override;

        /** Body::RemoveCollider */
        virtual void RemoveCollider(const HCollider& collider) override;

        /** Body::CheckForNestedBody */
        virtual void CheckForNestedBody() override;

        /** Body::ProcessCollisionData */
        void ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output) override;

        /** @copydoc Body::UpdateMassDistribution */
        void UpdateMassDistribution() override;

    protected:
        CSoftBody(); // Serialization only
    };
}
