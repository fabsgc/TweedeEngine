#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    class FPhysicsMesh;

    /**
     * Represents a physics mesh that can be used with a MeshCollider. Physics mesh can be a generic triangle mesh
     * or a convex mesh. Convex meshes are limited to a few hundred faces.
     */
    class TE_CORE_EXPORT PhysicsMesh : public Resource
    {
    public:
        PhysicsMesh(const SPtr<MeshData>& meshData);
        virtual ~PhysicsMesh() = default;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_PhysicsMesh; }

        /** Returns the mesh's indices and vertices. */
        SPtr<MeshData> GetMeshData() const;

        /**
         * Creates a new physics mesh.
         *
         * @param[in]	meshData	Index and vertices of the mesh data.
         */
        static HPhysicsMesh Create(const SPtr<MeshData>& meshData);

        /**
         * @copydoc Create()
         *
         * For internal use. Requires manual initialization after creation.
         */
        static SPtr<PhysicsMesh> _createPtr(const SPtr<MeshData>& meshData);

        /** Returns the internal implementation of the physics mesh. */
        virtual FPhysicsMesh* _getInternal() { return _internal.get(); }
    
    protected:
        SPtr<FPhysicsMesh> _internal;
        SPtr<MeshData> _initMeshData; // Transient, only used during initalization
    };

    /** Foundation that contains a specific implementation of a PhysicsMesh. */
    class TE_CORE_EXPORT FPhysicsMesh : public Serializable
    {
    public:
        FPhysicsMesh(const SPtr<MeshData>& meshData, UINT32 TID_type);
        virtual ~FPhysicsMesh() = default;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_FPhysicsMesh; }

        /** Re@copydoc PhysicsMesh::PhysicsMesh */
        SPtr<MeshData> GetMeshData() const { return _meshData;  }

    protected:
        friend class PhysicsMesh;

        SPtr<MeshData> _meshData = nullptr;
    };
}
