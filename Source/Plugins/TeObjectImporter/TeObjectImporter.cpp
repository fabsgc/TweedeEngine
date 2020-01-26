#include "TeObjectImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Mesh/TeMesh.h"
#include "Mesh/TeMeshData.h"

namespace te
{
    SPtr<Resource> GetTempMesh(SPtr<const ImportOptions> importOptions);

    ObjectImporter::ObjectImporter()
    {
        _extensions.push_back(u8"obj");
        _extensions.push_back(u8"dae");
        _extensions.push_back(u8"fbx");
    }

    ObjectImporter::~ObjectImporter()
    { }

    bool ObjectImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), tolower);

        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> ObjectImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<MeshImportOptions>();
    }

    SPtr<Resource> ObjectImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        MESH_DESC desc;
        Assimp::Importer importer;
        const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());

        const aiScene* scene = importer.ReadFile(filePath.c_str(),
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

        TE_ASSERT_ERROR(scene != nullptr, "Failed to load object '" + filePath + "' : " + importer.GetErrorString(), __FILE__, __LINE__);

        desc.Usage = MU_STATIC;
        if (meshImportOptions->CpuCached)
        {
            desc.Usage |= MU_CPUCACHED;
        }

        SPtr<RendererMeshData> rendererMeshData = ImportMeshData(scene, importOptions, desc.SubMeshes);
        //SPtr<MeshData> meshData = rendererMeshData->GetData();
        //SPtr<Mesh> mesh = Mesh::_createPtr(rendererMeshData->GetData(), desc);
        //SPtr<Mesh> mesh = Mesh::_createPtr(MESH_DESC());
        //mesh->SetName(filePath);
        
        return GetTempMesh(importOptions);
    }

    SPtr<RendererMeshData> ObjectImporter::ImportMeshData(const aiScene* scene, SPtr<const ImportOptions> importOptions, Vector<SubMesh>& subMeshes)
    {
        const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());
        Vector<SPtr<MeshData>> allMeshData;
        Vector<Vector<SubMesh>> allSubMeshes;

        if (scene->HasMaterials())
        {
        }

        if (scene->HasMeshes())
        {
        }

        return nullptr;
    }

    SPtr<Resource> GetTempMesh(SPtr<const ImportOptions> importOptions)
    {
        const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());

        MESH_DESC meshDesc;
        meshDesc.Usage = MU_STATIC;

        if (meshImportOptions->CpuCached)
        {
            meshDesc.Usage |= MU_CPUCACHED;
        }

        // ###################
        SPtr<VertexDataDesc> vertexDataxDesc = VertexDataDesc::Create();
        vertexDataxDesc->AddVertElem(VET_FLOAT4, VES_POSITION);

        meshDesc.NumVertices = 3;
        meshDesc.NumIndices = 3;
        meshDesc.Usage = MU_STATIC | MU_CPUCACHED;
        meshDesc.VertexDesc = vertexDataxDesc;

        HMesh mesh = Mesh::Create(meshDesc);
        SPtr<MeshData> meshData = MeshData::Create(3, 3, vertexDataxDesc);

        Vector4 vertexPositions[3];
        for (UINT32 i = 0; i < 3; i++)
        {
            vertexPositions[i] = Vector4((float)i, (float)i, (float)i, 1.0f);
        }
        // Write the vertices
        meshData->SetVertexData(VES_POSITION, (UINT8*)vertexPositions, sizeof(vertexPositions));

        UINT32* indices = meshData->GetIndices32();
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        // ###################

        mesh->WriteData(*meshData, false, true);
        mesh->SetName("dummy mesh");
        return mesh.GetInternalPtr();
    }
}
