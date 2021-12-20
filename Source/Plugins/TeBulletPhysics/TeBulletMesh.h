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
        struct MeshInfo
        {
            btScalar* Vertices = nullptr;
            int* Indices = nullptr;
            UINT32 NumTriangles = 0;
            UINT32 NumVertices = 0;
            UINT32 NumIndices = 0;
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
