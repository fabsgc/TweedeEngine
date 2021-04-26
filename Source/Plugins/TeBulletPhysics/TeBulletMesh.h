#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsMesh.h"

namespace te
{
    /** Bullet implementation of a PhysicsMesh. */
    class BulletMesh : public PhysicsMesh
    {
    public:
        BulletMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type);

    private:
        /** @copydoc PhysicsMesh::Initialize() */
        void Initialize() override;

        /** @copydoc PhysicsMesh::Destroy() */
        void Destroy() override;
    };

    /** PhysX implementation of the PhysicsMesh foundation, FPhysicsMesh. */
    class BulletFMesh : public FPhysicsMesh
    {
    public:
        BulletFMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type);
        ~BulletFMesh();

        /** @copydoc PhysicsMesh::getMeshData */
        SPtr<MeshData> GetMeshData() const override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_FBulletMesh; }

    private:
        /** Creates the internal triangle/convex mesh */
        void Initialize();

    public:
        BulletFMesh(); // Serialization only
    };
}
