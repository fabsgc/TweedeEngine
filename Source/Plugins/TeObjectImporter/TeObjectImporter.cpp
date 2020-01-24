#include "TeObjectImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Mesh/TeMesh.h"
#include "Mesh/TeMeshData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace te
{
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
        Assimp::Importer importer;
        const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());

        const aiScene* scene = importer.ReadFile(filePath.c_str(),
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

        TE_ASSERT_ERROR(scene != nullptr, "Failed to load object '" + filePath + "' : " + importer.GetErrorString(), __FILE__, __LINE__);

        if (scene->HasMaterials())
        {
        }

        if (scene->HasMeshes())
        {
        }

        MESH_DESC meshDesc;
        meshDesc.Usage = MU_STATIC;

        if (meshImportOptions->CpuCached)
        {
            meshDesc.Usage |= MU_CPUCACHED;
        }

        // ###################
        SPtr<VertexDataDesc> vertexDataxDesc = VertexDataDesc::Create();
        vertexDataxDesc->AddVertElem(VET_FLOAT4, VES_POSITION);
        vertexDataxDesc->AddVertElem(VET_FLOAT4, VES_COLOR);

        meshDesc.NumVertices = 4;
        meshDesc.NumIndices = 6;
        meshDesc.Usage = MU_STATIC | MU_CPUCACHED;
        meshDesc.VertexDesc = vertexDataxDesc;

        HMesh mesh = Mesh::Create(meshDesc);
        SPtr<MeshData> meshData = MeshData::Create(4, 6, vertexDataxDesc);

        Vector4 vertexPositions[4];
        for (UINT32 i = 0; i < 4; i++)
        {
            vertexPositions[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        Vector4 vertexColor[4];
        for (UINT32 i = 0; i < 4; i++)
        {
            vertexColor[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        // Write the vertices
        meshData->SetVertexData(VES_POSITION, (UINT8*)vertexPositions, sizeof(vertexPositions));
        meshData->SetVertexData(VES_COLOR, (UINT8*)vertexColor, sizeof(vertexColor));

        UINT32* indices = meshData->GetIndices32();
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        indices[3] = 2;
        indices[4] = 1;
        indices[5] = 3;
        // ###################

        mesh->WriteData(*meshData, true);
        mesh->SetName(filePath);
        return mesh.GetInternalPtr();
    }
}
