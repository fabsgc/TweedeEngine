#include "Physics/TePhysicsMesh.h"
#include "Resources/TeResourceManager.h"
#include "Physics/TePhysics.h"

namespace te
{
    PhysicsMesh::PhysicsMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type)
        : Resource(TID_Texture)
        , _initMeshData(meshData)
        , _type(type)
    {
        // Derived class is responsible for initializing mInternal
    }

    PhysicsMeshType PhysicsMesh::GetType() const
    {
        return _internal->_type;
    }

    SPtr<MeshData> PhysicsMesh::GetMeshData() const
    {
        return _internal->GetMeshData();
    }

    HPhysicsMesh PhysicsMesh::Create(const SPtr<MeshData>& meshData, PhysicsMeshType type)
    {
        SPtr<PhysicsMesh> newMesh = _createPtr(meshData, type);
        return static_resource_cast<PhysicsMesh>(gResourceManager()._createResourceHandle(newMesh));
    }

    SPtr<PhysicsMesh> PhysicsMesh::_createPtr(const SPtr<MeshData>& meshData, PhysicsMeshType type)
    {
        SPtr<PhysicsMesh> newMesh = gPhysics().CreateMesh(meshData, type);
        newMesh->SetThisPtr(newMesh);
        newMesh->Initialize();

        return newMesh;
    }

    void PhysicsMesh::Initialize()
    {
        _initMeshData = nullptr;
        Resource::Initialize();
    }

    FPhysicsMesh::FPhysicsMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type, UINT32 TID_type)
        : Serializable(TID_type)
        , _type(type)
    { }
}
