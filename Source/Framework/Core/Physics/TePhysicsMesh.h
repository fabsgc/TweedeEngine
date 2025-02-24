#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    /**
     * Represents a physics mesh that can be used with a MeshCollider. Physics mesh can be a generic triangle mesh
     * or a convex mesh. Convex meshes are limited to a few hundred faces.
     */
    class TE_CORE_EXPORT PhysicsMesh : public Resource
    {
    public:
        explicit PhysicsMesh(const SPtr<MeshData>& meshData);
        virtual ~PhysicsMesh() = 0;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**  @copydoc Resource::GetResourceType */
        static CoreType GetResourceType() { return CoreType::TID_PhysicsMesh; }

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
        static SPtr<PhysicsMesh> CreatePtr(const SPtr<MeshData>& meshData);

        /**	Creates a new empty object but doesn't initialize it. */
        static SPtr<PhysicsMesh> CreateEmpty();

        /** Returns the internal implementation of the physics mesh. */
        virtual FPhysicsMesh* GetInternal() { return _internal.get(); }

    public:
        void Serialize(StreamWriter* serializer) const override;

        static bool Deserialize(StreamReader* deserializer, PhysicsMesh* object);

    protected:
        SPtr<FPhysicsMesh> _internal;
        SPtr<MeshData> _initMeshData; // Transient, only used during initalization
    };
}
