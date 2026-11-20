#include "Physics/TePhysicsMesh.h"
#include "Physics/TeFPhysicsMesh.h"
#include "Resources/TeResourceManager.h"
#include "Physics/TePhysics.h"
#include "Mesh/TeMeshData.h"
#include "RenderAPI/TeVertexDataDesc.h"

#include <limits>

namespace te
{
    PhysicsMesh::PhysicsMesh(const SPtr<MeshData>& meshData)
        : Resource(CoreType::TID_PhysicsMesh)
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

    SPtr<PhysicsMesh> PhysicsMesh::CreateEmpty()
    {
        SPtr<PhysicsMesh> mesh = gPhysics().CreateMesh(nullptr);
        if (mesh)
            mesh->SetThisPtr(mesh);

        return mesh;
    }

    void PhysicsMesh::Initialize()
    {
        Resource::Initialize();
        // _initMeshData = nullptr; not sure for serialization purpose ?
    }

    void PhysicsMesh::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        SPtr<MeshData> meshData = _internal ? _internal->GetMeshData() : _initMeshData;
        TE_ASSERT_ERROR(meshData != nullptr && meshData->GetVertexDesc() != nullptr,
            "Cannot serialize a physics mesh without mesh data.");
        if (!meshData || !meshData->GetVertexDesc())
            return;

        const SPtr<VertexDataDesc>& vertexDesc = meshData->GetVertexDesc();
        serializer->WriteRaw<UINT32>(meshData->GetNumVertices());
        serializer->WriteRaw<UINT32>(meshData->GetNumIndices());
        serializer->WriteRaw<UINT32>(static_cast<UINT32>(meshData->GetIndexType()));
        serializer->WriteRaw<UINT32>(vertexDesc->GetNumElements());

        for (UINT32 i = 0; i < vertexDesc->GetNumElements(); i++)
        {
            const VertexElement& element = vertexDesc->GetElement(i);
            serializer->WriteRaw<UINT32>(static_cast<UINT32>(element.GetType()));
            serializer->WriteRaw<UINT32>(static_cast<UINT32>(element.GetSemantic()));
            serializer->WriteRaw<UINT32>(element.GetSemanticIdx());
            serializer->WriteRaw<UINT32>(element.GetStreamIdx());
            serializer->WriteRaw<UINT32>(element.GetInstanceStepRate());
        }

        serializer->WriteBuffer(meshData->GetData(), meshData->GetSize());
    }

    bool PhysicsMesh::Deserialize(StreamReader* deserializer, PhysicsMesh* object)
    {
        if (!deserializer || !object)
            return false;

        if (!Resource::Deserialize(deserializer, object))
            return false;

        UINT32 numVertices = 0;
        UINT32 numIndices = 0;
        UINT32 indexTypeValue = 0;
        UINT32 numElements = 0;
        deserializer->ReadRaw<UINT32>(numVertices);
        deserializer->ReadRaw<UINT32>(numIndices);
        deserializer->ReadRaw<UINT32>(indexTypeValue);
        deserializer->ReadRaw<UINT32>(numElements);
        if (!deserializer->IsStreamGood() || indexTypeValue > static_cast<UINT32>(IT_32BIT))
            return false;

        const IndexType indexType = static_cast<IndexType>(indexTypeValue);
        SPtr<VertexDataDesc> vertexDesc = VertexDataDesc::Create();
        UINT64 vertexStride = 0;

        for (UINT32 i = 0; i < numElements; i++)
        {
            UINT32 elementType = 0;
            UINT32 semantic = 0;
            UINT32 semanticIdx = 0;
            UINT32 streamIdx = 0;
            UINT32 instanceStepRate = 0;
            deserializer->ReadRaw<UINT32>(elementType);
            deserializer->ReadRaw<UINT32>(semantic);
            deserializer->ReadRaw<UINT32>(semanticIdx);
            deserializer->ReadRaw<UINT32>(streamIdx);
            deserializer->ReadRaw<UINT32>(instanceStepRate);

            if (!deserializer->IsStreamGood() ||
                elementType >= static_cast<UINT32>(VET_COUNT) ||
                semantic < static_cast<UINT32>(VES_POSITION) ||
                semantic > static_cast<UINT32>(VES_PSIZE))
            {
                return false;
            }

            const VertexElementType type = static_cast<VertexElementType>(elementType);
            vertexDesc->AddVertElem(type, static_cast<VertexElementSemantic>(semantic), semanticIdx, streamIdx, instanceStepRate);
            vertexStride += VertexElement::GetTypeSize(type);
        }

        const UINT64 indexBufferSize = static_cast<UINT64>(numIndices) *
            (indexType == IT_16BIT ? sizeof(UINT16) : sizeof(UINT32));
        const UINT64 meshBufferSize = indexBufferSize + static_cast<UINT64>(numVertices) * vertexStride;
        if (meshBufferSize > std::numeric_limits<UINT32>::max())
            return false;

        SPtr<MeshData> meshData = MeshData::Create(numVertices, numIndices, vertexDesc, indexType);
        if (meshData->GetSize() != meshBufferSize)
            return false;

        size_t serializedBufferSize = 0;
        deserializer->ReadRaw<size_t>(serializedBufferSize);
        if (!deserializer->IsStreamGood() || serializedBufferSize != meshData->GetSize())
            return false;
        if (!deserializer->ReadData(meshData->GetData(), serializedBufferSize))
            return false;

        object->_initMeshData = meshData;
        object->Initialize();

        return object->_internal != nullptr;
    }
}
