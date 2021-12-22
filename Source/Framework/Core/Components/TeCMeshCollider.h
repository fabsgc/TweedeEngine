#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeMeshCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	MeshCollider
     *
     * @note	Wraps MeshCollider as a Component.
     */
    class TE_CORE_EXPORT CMeshCollider : public CCollider
    {
    public:
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CMeshCollider; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HMeshCollider& c, const String& suffix = "");

        /** @copydoc MeshCollider::SetMesh */
        void SetMesh(const HPhysicsMesh& mesh);

        /** @copydoc MeshCollider::GetMesh */
        HPhysicsMesh GetMesh() const { return _mesh; }

        void SetCollisionType(PhysicsMeshType type);

        PhysicsMeshType GetCollisionType() const { return _collisionType; }

    protected:
        friend class SceneObject;

        CMeshCollider(); // Serialization only
        CMeshCollider(const HSceneObject& parent);

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /** @copydoc CCollider::IsValidParent */
        bool IsValidParent(const HRigidBody& parent) const override;

        /**	Returns the mesh collider that this component wraps. */
        MeshCollider* GetInternal() const { return static_cast<MeshCollider*>(_internal.get()); }

    protected:
        HPhysicsMesh _mesh;

        PhysicsMeshType _collisionType = PhysicsMeshType::Convex;
    };
}
