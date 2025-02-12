#include "Physics/TeFPhysicsMesh.h"

namespace te
{
    FPhysicsMesh::FPhysicsMesh(const SPtr<MeshData>& meshData)
        : Serializable(CoreType::TID_FPhysicsMesh)
        , _meshData(meshData)
    { }
}
