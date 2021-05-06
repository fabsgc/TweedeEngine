#include "Physics/TePhysicsMesh.h"
#include "Resources/TeResourceManager.h"
#include "Physics/TePhysics.h"

namespace te
{
    PhysicsMesh::PhysicsMesh(const SPtr<MeshData>& meshData)
        : Resource(TID_PhysicsMesh)
        , _initMeshData(meshData)
    {
        // Derived class is responsible for initializing _internal
    }

    SPtr<MeshData> PhysicsMesh::GetMeshData() const
    {
        return _internal->GetMeshData();
    }

    HPhysicsMesh PhysicsMesh::Create(const SPtr<MeshData>& meshData)
    {
        if (meshData)
        {
            SPtr<PhysicsMesh> newMesh = _createPtr(meshData);
            return static_resource_cast<PhysicsMesh>(gResourceManager()._createResourceHandle(newMesh));
        }

        return HPhysicsMesh();
    }

    SPtr<PhysicsMesh> PhysicsMesh::_createPtr(const SPtr<MeshData>& meshData)
    {
        if (meshData)
        {
            SPtr<PhysicsMesh> newMesh = gPhysics().CreateMesh(meshData);
            newMesh->SetThisPtr(newMesh);
            newMesh->Initialize();

            return newMesh;
        }

        return nullptr;
    }

    void PhysicsMesh::Initialize()
    {
        _initMeshData = nullptr;
        Resource::Initialize();
    }

    FPhysicsMesh::FPhysicsMesh(const SPtr<MeshData>& meshData, UINT32 TID_type)
        : Serializable(TID_type)
        , _meshData(meshData)
    { }
}
