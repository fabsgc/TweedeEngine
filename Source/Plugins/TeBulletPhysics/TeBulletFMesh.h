#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFPhysicsMesh.h"
#include "TeBulletMesh.h"

namespace te
{
    /** Bullet implementation of the PhysicsMesh foundation, FPhysicsMesh. */
    class BulletFMesh : public FPhysicsMesh
    {
    public:
        explicit BulletFMesh(const SPtr<MeshData>& meshData);
        ~BulletFMesh();

        /** Returns mesh generated data */
        SPtr<BulletMesh::MeshInfo> GetMeshInfo() const { return _meshInfo; }

    private:
        /** Creates the internal triangle/convex mesh */
        void Initialize();

    private:
        SPtr<BulletMesh::MeshInfo> _meshInfo = nullptr;
    };
}
