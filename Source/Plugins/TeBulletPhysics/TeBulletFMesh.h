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
        BulletFMesh(const SPtr<MeshData>& meshData);
        ~BulletFMesh();

        /** Returns convex mesh generated data */
        SPtr<BulletMesh::ConvexMesh> GetConvexMesh() const { return _convexMesh; }

        /** Returns triangle mesh generated data */
        SPtr<BulletMesh::TriangleMesh> GetTriangleMesh() const { return _triangleMesh; }

        /** Returns triangle mesh generated data */
        SPtr<BulletMesh::SoftBodyMesh> GetSoftBodyMesh() const { return _softBodyMesh; }

    private:
        /** Creates the internal triangle/convex mesh */
        void Initialize();

    public:
        BulletFMesh(); // Serialization only

    private:
        SPtr<BulletMesh::ConvexMesh> _convexMesh = nullptr;
        SPtr<BulletMesh::TriangleMesh> _triangleMesh = nullptr;
        SPtr<BulletMesh::SoftBodyMesh> _softBodyMesh = nullptr;
    };
}
