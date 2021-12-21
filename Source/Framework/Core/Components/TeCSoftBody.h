#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSoftBody.h"
#include "Components/TeCBody.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysicsMesh.h"

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
        static UINT32 GetComponentType() { return TypeID_Core::TID_CSoftBody; }

        /** @copydoc SoftBody::SetMesh */
        void SetMesh(const HPhysicsMesh& mesh);

        /** @copydoc SoftBody::GetMesh */
        HPhysicsMesh GetMesh() const { return _mesh; }

        /** @copydoc SoftBody::GetMesh */
        void SetScale(const Vector3& scale);

        /** @copydoc SoftBody::SetScale */
        const Vector3& GetScale() const { return _scale; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        bool Clone(const HSoftBody& c, const String& suffix = "");

        /** @copydoc Component::Update */
        void Update() override;

        /** Returns the SoftBody implementation wrapped by this component. */
        Body* GetInternal() const override { return (SoftBody*)_internal.get(); }

    protected:
        friend class SceneObject;
        friend class CCollider;

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

        /** Destroys the internal SoftBody representation. */
        virtual void DestroyInternal() override;

        /** Body::ProcessCollisionData */
        void ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output) override;

    protected:
        CSoftBody(); // Serialization only

    protected:
        HPhysicsMesh _mesh;
        Vector3 _scale = Vector3::ONE;
    };
}
