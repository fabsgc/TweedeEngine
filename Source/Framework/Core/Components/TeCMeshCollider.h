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
        CMeshCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CMeshCollider; }

        /** @copydoc MeshCollider::SetMesh() */
        void SetMesh(const HPhysicsMesh& mesh);

        /** @copydoc MeshCollider::GetMesh() */
        HPhysicsMesh GetMesh() const { return _mesh; }

        /** @copydoc Component::Clone */
        void Clone(const HMeshCollider& c);

        void SetCollisionType(PhysicsMeshType type);

        PhysicsMeshType GetCollisionType() const { return _collisionType; }

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal() */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal() */
        void RestoreInternal() override;

        /** @copydoc CCollider::IsValidParent */
        bool IsValidParent(const HBody& parent) const override;

        /**	Returns the mesh collider that this component wraps. */
        MeshCollider* _getInternal() const { return static_cast<MeshCollider*>(_internal.get()); }

    protected:
        CMeshCollider(); // Serialization only

    protected:
        HPhysicsMesh _mesh;

        PhysicsMeshType _collisionType = PhysicsMeshType::Convex;
    };
}
