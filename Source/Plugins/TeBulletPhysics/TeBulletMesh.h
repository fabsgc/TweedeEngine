#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFPhysicsMesh.h"
#include "Physics/TePhysicsMesh.h"

namespace te
{
    /** Bullet implementation of a PhysicsMesh. */
    class BulletMesh : public PhysicsMesh
    {
    public:
        struct ConvexMesh
        {
            UINT8* Vertices = 0;
            UINT32 NumVertices = 0;
            UINT32 Stride = 0;
        };

        struct TriangleMesh
        {
            UINT8* Vertices = nullptr;
            UINT8* Indices = nullptr;
            UINT32 NumVertices = 0;
            UINT32 NumIndices = 0;
            UINT32 VertexStride = 0;
            UINT32 IndexStride = 0;
            bool   Use32BitIndex = true;
        };

        struct SoftBodyMesh
        {
            btScalar* Vertices = nullptr;
            int* Indices = nullptr;
            UINT32 NumTriangles = 0;
        };

    public:
        BulletMesh(const SPtr<MeshData>& meshData);

    private:
        /** @copydoc PhysicsMesh::Initialize */
        void Initialize() override;

        /** @copydoc PhysicsMesh::Destroy */
        void Destroy() override;
    };
}
