#include "TeObjectImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Mesh/TeMesh.h"
#include "Mesh/TeMeshData.h"
#include "Utility/TeColor.h"

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
        const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());

        desc.Usage = MU_STATIC;
        if (meshImportOptions->CpuCached)
        {
            desc.Usage |= MU_CPUCACHED;
        }

        SPtr<RendererMeshData> rendererMeshData = ImportMeshData(filePath, importOptions, desc.SubMeshes);
        //SPtr<MeshData> meshData = rendererMeshData->GetData();
        //SPtr<Mesh> mesh = Mesh::_createPtr(rendererMeshData->GetData(), desc);
        //SPtr<Mesh> mesh = Mesh::_createPtr(MESH_DESC());
        //mesh->SetName(filePath);
        
        return GetTempMesh(importOptions);
    }

    SPtr<RendererMeshData> ObjectImporter::ImportMeshData(const String& filePath, SPtr<const ImportOptions> importOptions, Vector<SubMesh>& subMeshes)
    {
        Assimp::Importer importer;
        AssimpImportScene importedScene;
        const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());

        unsigned int assimpFlags = aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenNormals |
            aiProcess_GenUVCoords |
            aiProcess_FlipUVs |
            aiProcess_FlipWindingOrder |
            aiProcess_SortByPType;

        if (meshImportOptions->SplitUV)
        {
            assimpFlags |= aiProcess_FlipUVs;
        }

        if (meshImportOptions->LeftHanded)
        {
            assimpFlags |= aiProcess_MakeLeftHanded;
        }

        aiScene* scene = const_cast<aiScene*>(importer.ReadFile(filePath.c_str(), assimpFlags));

        TE_ASSERT_ERROR(scene != nullptr, "Failed to load object '" + filePath + "' : " + importer.GetErrorString(), __FILE__, __LINE__);

        AssimpImportOptions assimpImportOptions;
        assimpImportOptions.ImportNormals = meshImportOptions->ImportNormals;
        assimpImportOptions.ImportTangents = meshImportOptions->ImportTangents;
        assimpImportOptions.ImportScale = meshImportOptions->ImportScale;

        ParseScene(scene, assimpImportOptions, importedScene);

        SPtr<RendererMeshData> rendererMeshData = GenerateMeshData(importedScene, assimpImportOptions, subMeshes);

        return rendererMeshData;
    }

    void ObjectImporter::ParseScene(aiScene* scene, const AssimpImportOptions& options, AssimpImportScene& outputScene)
    {
        outputScene.RootNode = CreateImportNode(outputScene, scene->mRootNode, nullptr);

        Stack<aiNode*> todo;
        todo.push(scene->mRootNode);

        while (!todo.empty())
        {
            aiNode* curNode = todo.top();
            AssimpImportNode* curImportNode = outputScene.NodeMap[curNode];
            todo.pop();

            curImportNode->Name = curNode->mName.C_Str();

            for (unsigned int i = 0; i < curNode->mNumMeshes; i++)
            {
                unsigned int meshIndex = curNode->mMeshes[i];
                aiMesh* mesh = scene->mMeshes[meshIndex];
                ParseMesh(mesh, curImportNode, options, outputScene);
            }

            for (unsigned int i = 0; i < curNode->mNumChildren; i++)
            {
                aiNode* childNode = curNode->mChildren[i];
                CreateImportNode(outputScene, childNode, curImportNode);

                todo.push(childNode);
            }
        }
    }

    void ObjectImporter::ParseMesh(aiMesh* mesh, AssimpImportNode* parentNode, const AssimpImportOptions& options, AssimpImportScene& outputScene)
    {
        unsigned int vertexCount = mesh->mNumVertices;
        unsigned int triangleCount = mesh->mNumFaces;

        if (vertexCount == 0 || triangleCount == 0)
            return;

        // Register in global mesh array
        AssimpImportMesh* importMesh = nullptr;

        auto iterFindMesh = outputScene.MeshMap.find(mesh);
        if (iterFindMesh != outputScene.MeshMap.end())
        {
            UINT32 meshIdx = iterFindMesh->second;
            outputScene.Meshes[meshIdx]->ReferencedBy.push_back(parentNode);

            return;
        }
        else
        {
            importMesh = te_new<AssimpImportMesh>();
            outputScene.Meshes.push_back(importMesh);

            importMesh->ReferencedBy.push_back(parentNode);
            importMesh->AssimpMesh = mesh;

            outputScene.MeshMap[mesh] = (UINT32)outputScene.Meshes.size() - 1;
        }

        // Import vertices

        // Import triangles

        // Import UVs

        // Import tangents
    }

    SPtr<RendererMeshData> ObjectImporter::GenerateMeshData(AssimpImportScene& scene, AssimpImportOptions& options, Vector<SubMesh> subMeshes)
    {
        Vector<SPtr<MeshData>> allMeshData;
        Vector<Vector<SubMesh>> allSubMeshes;

        return nullptr;
    }

    AssimpImportNode* ObjectImporter::CreateImportNode(AssimpImportScene& scene, aiNode* assimpNode, AssimpImportNode* parent)
    {
        AssimpImportNode* node = te_new<AssimpImportNode>();
        aiMatrix4x4 assimpWorldTransform = assimpNode->mTransformation;

        node->LocalTransform = ConvertToNativeType(assimpNode->mTransformation);

        if (parent != nullptr)
        {
            node->WorldTransform = parent->WorldTransform * node->LocalTransform;
            parent->Children.push_back(node);
        }
        else
        {
            node->WorldTransform = node->LocalTransform;
        }

        scene.NodeMap.insert(std::make_pair(assimpNode, node));

        return node;
    }

    Matrix4 ObjectImporter::ConvertToNativeType(const aiMatrix4x4 matrix)
    {
        return Matrix4(
            (float)matrix.a1, (float)matrix.a2, (float)matrix.a3, (float)matrix.a4,
            (float)matrix.b1, (float)matrix.b2, (float)matrix.b3, (float)matrix.b4,
            (float)matrix.c1, (float)matrix.c2, (float)matrix.c3, (float)matrix.c4,
            (float)matrix.d1, (float)matrix.d2, (float)matrix.d3, (float)matrix.d4
        );
    }

    Color ObjectImporter::ConvertToNativeType(const aiColor4D color)
    {
        return Color((float)color.a, (float)color.g, (float)color.b, (float)color.a);
    }

    Vector3 ObjectImporter::ConvertToNativeType(const aiVector3D vector)
    {
        return Vector3((float)vector.x, (float)vector.y, (float)vector.z);
    }

    Vector2 ObjectImporter::ConvertToNativeType(const aiVector2D vector)
    {
        return Vector2((float)vector.x, (float)vector.y);
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
