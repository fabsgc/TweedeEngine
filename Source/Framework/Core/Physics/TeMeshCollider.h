#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with mesh geometry. */
    class TE_CORE_EXPORT MeshCollider : public Collider
    {
    public:
        MeshCollider() = default;
        ~MeshCollider() = default;

        /**
         * Sets a mesh that represents the collider geometry. This can be a generic triangle mesh, or and convex mesh.
         */
        void SetPhysicMesh(const HPhysicsMesh& physicMesh) { _physicMesh = physicMesh; OnMeshChanged(); }

        /** @copydoc SetPhysicMesh() */
        HPhysicsMesh GetPhysicMesh() const { return _physicMesh; }

        /**
         * Type of MeshCollider created behind
         *
         * @param[in]	type		Type of the mesh. If convex the provided mesh geometry will be converted into a convex
         *							mesh (that might not be the same as the provided mesh data).
         */
        virtual void SetCollisionType(PhysicsMeshType type) { _collisionType = type; }

        /** @copydoc SetCollisionType */
        PhysicsMeshType GetCollisionType() { return _collisionType; }

        /**
         * Creates a new mesh collider.
         *
         * @param[in]	scene		Scene into which to add the collider to.
         * @param[in]	position	Position of the collider relative to its parent
		 * @param[in]	rotation	Position of the collider relative to its parent
         */
        static SPtr<MeshCollider> Create(PhysicsScene& scene, const Vector3& position = Vector3::ZERO,
            const Quaternion& rotation = Quaternion::IDENTITY);

    protected:
        /**
         * Triggered by the resources system whenever the attached collision mesh changed (e.g. was reimported) or loaded.
         */
        virtual void OnMeshChanged() { }

    protected:
        HPhysicsMesh _physicMesh;

        PhysicsMeshType _collisionType = PhysicsMeshType::Convex;
    };
}
