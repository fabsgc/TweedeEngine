#include "Physics/TeFPhysicsMesh.h"

namespace te
{
    FPhysicsMesh::FPhysicsMesh(const SPtr<MeshData>& meshData)
        : Serializable(TID_FPhysicsMesh)
        , _meshData(meshData)
    { }
}
