#include "TeBulletMesh.h"
#include "TeBulletFMesh.h"

namespace te
{
    BulletMesh::BulletMesh(const SPtr<MeshData>& meshData)
        : PhysicsMesh(meshData)
    { }

    void BulletMesh::Initialize()
    {
        if (_internal == nullptr) // Could be not-null if we're deserializing
            _internal = te_shared_ptr_new<BulletFMesh>(_initMeshData);

        PhysicsMesh::Initialize();
    }

    void BulletMesh::Destroy()
    {
        _internal = nullptr;
        PhysicsMesh::Destroy();
    }
}
