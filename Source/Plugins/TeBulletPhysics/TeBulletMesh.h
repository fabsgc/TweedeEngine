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
            UINT8* Data = 0;
            UINT32 NumVertices = 0;
            UINT32 Stride = 0;
        };

        struct TriangleMesh
        {
            UINT8* Data = nullptr;
            UINT8* Indices = nullptr;
            UINT32 NumVertices = 0;
            UINT32 NumIndices = 0;
            UINT32 VerticeStride = 0;
            UINT32 IndexStride = 0;
            bool   Use32BitIndex = true;
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
