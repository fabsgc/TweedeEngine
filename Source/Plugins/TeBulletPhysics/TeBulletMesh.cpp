#include "TeBulletMesh.h"
#include "Mesh/TeMeshData.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "TeBulletPhysics.h"
#include "Math/TeAABox.h"

namespace te
{
    BulletMesh::BulletMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type)
        : PhysicsMesh(meshData, type)
    { }

    void BulletMesh::Initialize()
    {
        if (_internal == nullptr) // Could be not-null if we're deserializing
            _internal = te_shared_ptr_new<BulletFMesh>(_initMeshData, _type);

        PhysicsMesh::Initialize();
    }

    void BulletMesh::Destroy()
    {
        _internal = nullptr;
        PhysicsMesh::Destroy();
    }

    BulletFMesh::BulletFMesh()
        : FPhysicsMesh(nullptr, PhysicsMeshType::Convex, TID_FBulletMesh)
    { }

    BulletFMesh::BulletFMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type)
        : FPhysicsMesh(meshData, type, TID_FBulletMesh)
    {
        // Perform cooking if needed
        if (meshData != nullptr)
        {
            // TODO
        }

        Initialize();
    }

    BulletFMesh::~BulletFMesh()
    {
        // TODO
    }

    void BulletFMesh::Initialize()
    {
        // TODO
    }

    SPtr<MeshData> BulletFMesh::GetMeshData() const
    {
        return nullptr;
    }
}
