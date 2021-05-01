#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsMesh.h"

namespace te
{
    /** Bullet implementation of a PhysicsMesh. */
    class BulletMesh : public PhysicsMesh
    {
    public:
        struct ConvexMesh
        {
            UINT8* Data;
            UINT32 NumVertices;
            UINT32 Stride;
        };

        struct TriangleMesh
        {
            // TODO
        };

    public:
        BulletMesh(const SPtr<MeshData>& meshData);

    private:
        /** @copydoc PhysicsMesh::Initialize() */
        void Initialize() override;

        /** @copydoc PhysicsMesh::Destroy() */
        void Destroy() override;
    };

    /** Bullet implementation of the PhysicsMesh foundation, FPhysicsMesh. */
    class BulletFMesh : public FPhysicsMesh
    {
    public:
        BulletFMesh(const SPtr<MeshData>& meshData);
        ~BulletFMesh();

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_FBulletMesh; }

        /** Returns convex mesh generated data */
        SPtr<BulletMesh::ConvexMesh> GetConvexMesh() const { return _convexMesh; }

        /** Returns triangle mesh generated data */
        SPtr<BulletMesh::TriangleMesh> GetTriangleMesh() const { return _triangleMesh; }

    private:
        /** Creates the internal triangle/convex mesh */
        void Initialize();

    public:
        BulletFMesh(); // Serialization only

    private:
        SPtr<BulletMesh::ConvexMesh> _convexMesh = nullptr;
        SPtr<BulletMesh::TriangleMesh> _triangleMesh = nullptr;
    };
}
