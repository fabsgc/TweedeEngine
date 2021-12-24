#include "Physics/TePhysicsMesh.h"
#include "Physics/TeFPhysicsMesh.h"
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

    PhysicsMesh::~PhysicsMesh()
    { }

    SPtr<MeshData> PhysicsMesh::GetMeshData() const
    {
        return _internal->GetMeshData();
    }

    HPhysicsMesh PhysicsMesh::Create(const SPtr<MeshData>& meshData)
    {
        if (meshData)
        {
            SPtr<PhysicsMesh> newMesh = CreatePtr(meshData);
            return static_resource_cast<PhysicsMesh>(gResourceManager()._createResourceHandle(newMesh));
        }

        return HPhysicsMesh();
    }

    SPtr<PhysicsMesh> PhysicsMesh::CreatePtr(const SPtr<MeshData>& meshData)
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
}
