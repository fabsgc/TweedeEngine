#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    /** Foundation that contains a specific implementation of a PhysicsMesh. */
    class TE_CORE_EXPORT FPhysicsMesh : public Serializable
    {
    public:
        FPhysicsMesh(const SPtr<MeshData>& meshData);
        virtual ~FPhysicsMesh() = default;

        /** @copydoc PhysicsMesh::GetMeshData */
        SPtr<MeshData> GetMeshData() const { return _meshData;  }

    protected:
        friend class PhysicsMesh;

        SPtr<MeshData> _meshData = nullptr;
    };
}
