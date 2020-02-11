#include "TeObjectImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Mesh/TeMesh.h"
#include "Mesh/TeMeshData.h"
#include "Image/TeColor.h"

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
        MESH_DESC desc;
        MeshImportOptions* meshImportOptions = const_cast<MeshImportOptions*>
            (static_cast<const MeshImportOptions*>(importOptions.get()));

        desc.Usage = MU_STATIC;
        if (meshImportOptions->CpuCached)
        {
            desc.Usage |= MU_CPUCACHED;
        }

        String extension = Util::GetFileExtension(filePath);
        std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
        if (extension == ".fbx")
        {
            meshImportOptions->ScaleSystemUnit = true;
            meshImportOptions->ScaleFactor = 0.01f;
        }

        SPtr<RendererMeshData> rendererMeshData = ImportMeshData(filePath, importOptions, desc.SubMeshes);
        SPtr<MeshData> meshData = rendererMeshData->GetData();
        SPtr<Mesh> mesh = Mesh::_createPtr(rendererMeshData->GetData(), desc);
        mesh->SetName(filePath);
        
        return mesh;
    }

    SPtr<RendererMeshData> ObjectImporter::ImportMeshData(const String& filePath, SPtr<const ImportOptions> importOptions, Vector<SubMesh>& subMeshes)
    {
        Assimp::Importer importer;
        AssimpImportScene importedScene;
        const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());

        unsigned int assimpFlags =
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_GenUVCoords |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType;

        if (meshImportOptions->FplitUV)
        {
            assimpFlags |= aiProcess_FlipUVs;
        }

        if (meshImportOptions->FlipWinding)
        {
            assimpFlags |= aiProcess_FlipWindingOrder;
        }

        if (meshImportOptions->LeftHanded)
        {
            assimpFlags |= aiProcess_MakeLeftHanded;
        }

        aiScene* scene = const_cast<aiScene*>(importer.ReadFile(filePath.c_str(), assimpFlags));

        TE_ASSERT_ERROR(scene != nullptr, "Failed to load object '" + filePath + "' : " + importer.GetErrorString(), __FILE__, __LINE__);

        AssimpImportOptions assimpImportOptions;
        assimpImportOptions.ImportNormals   = meshImportOptions->ImportNormals;
        assimpImportOptions.ImportTangents  = meshImportOptions->ImportTangents;
        assimpImportOptions.ImportSkin      = meshImportOptions->ImportSkin;
        assimpImportOptions.ScaleSystemUnit = meshImportOptions->ScaleSystemUnit;
        assimpImportOptions.ScaleFactor     = meshImportOptions->ScaleFactor;

        ParseScene(scene, assimpImportOptions, importedScene);

        SPtr<RendererMeshData> rendererMeshData = GenerateMeshData(importedScene, assimpImportOptions, subMeshes);

        return rendererMeshData;
    }

    void ObjectImporter::ParseScene(aiScene* scene, const AssimpImportOptions& options, AssimpImportScene& outputScene)
    {
        outputScene.RootNode = CreateImportNode(options, outputScene, scene->mRootNode, nullptr);

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
                CreateImportNode(options, outputScene, childNode, curImportNode);

                todo.push(childNode);
            }
        }

        if (scene->HasMaterials())
        {
            AssimpImportMaterial material;
            aiMaterial* mat = nullptr;

            for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            {
                mat = scene->mMaterials[i];
                if (!mat)
                    continue;

                aiString name;
                mat->Get(AI_MATKEY_NAME, name);

                material.Index = i;
                material.Name = name.C_Str();

                outputScene.Materials.push_back(material);
            }
        }
    }

    void ObjectImporter::ParseMesh(aiMesh* mesh, AssimpImportNode* parentNode, const AssimpImportOptions& options, AssimpImportScene& outputScene)
    {
        unsigned int vertexCount = mesh->mNumVertices;
        unsigned int triangleCount = mesh->mNumFaces;
        unsigned int indexCount = mesh->mNumFaces * 3;

        if (vertexCount == 0 || triangleCount == 0)
            return;

        if (!mesh->HasFaces())
            return;

        if (!(mesh->mPrimitiveTypes | aiPrimitiveType_TRIANGLE || mesh->mPrimitiveTypes | aiPrimitiveType_POLYGON))
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

        // Import colors
        if (options.ImportColors)
        {
            importMesh->Colors.resize(vertexCount);
        }

        // Import vertices
        importMesh->Positions.resize(vertexCount);
        for (UINT32 i = 0; i < vertexCount; i++)
        {
            importMesh->Positions[i] = ConvertToNativeType(mesh->mVertices[i]);

            if (options.ScaleSystemUnit)
            {
                importMesh->Positions[i] *= options.ScaleFactor;
            }

            if (mesh->HasVertexColors(i) && options.ImportColors)
            {
                importMesh->Colors[i] = ConvertToNativeType(*mesh->mColors[i]);
            }
            else
            {
                importMesh->Colors[i] = Color::LightGray.GetAsVector4();
            }
        }

        // Import triangles
        importMesh->Indices.resize(indexCount);
        for (UINT32 i = 0; i < triangleCount; i++)
        {
            aiFace* face = &mesh->mFaces[i];
            for (UINT32 j = 0; j < face->mNumIndices; j++)
            {
                importMesh->Indices[(i * 3) + j] = face->mIndices[j]; 
            }
        }

        // Import normals 
        if (mesh->HasNormals() && options.ImportNormals)
        {
            importMesh->Normals.resize(vertexCount);

            for (UINT32 i = 0; i < vertexCount; i++)
            {
                importMesh->Normals[i] = ConvertToNativeType(mesh->mNormals[i]);
            }
        }
        
        // Import tangents and bitangents
        if (mesh->HasTangentsAndBitangents() && options.ImportTangents)
        {
            importMesh->Tangents.resize(vertexCount);
            importMesh->Bitangents.resize(vertexCount);

            for (UINT32 i = 0; i < vertexCount; i++)
            {
                importMesh->Tangents[i] = ConvertToNativeType(mesh->mTangents[i]);
                importMesh->Bitangents[i] = ConvertToNativeType(mesh->mBitangents[i]);
            }
        }

        // Import UVs
        for (UINT32 i = 0; i < OBJECT_IMPORT_MAX_UV_LAYERS; i++)
        {
            if (!mesh->HasTextureCoords(i))
                break;

            importMesh->Textures[i].resize(vertexCount);
            for (UINT32 j = 0; j < vertexCount; j++)
            {
                Vector3 coord = ConvertToNativeType(mesh->mTextureCoords[i][j]);
                importMesh->Textures[i][j] = Vector2(coord.x, coord.y);
            }
        }

        // Import Materials
        importMesh->MaterialIndex = mesh->mMaterialIndex;
    }

    SPtr<RendererMeshData> ObjectImporter::GenerateMeshData(AssimpImportScene& scene, AssimpImportOptions& options, Vector<SubMesh>& outputSubMeshes)
    {
        Vector<SPtr<MeshData>> allMeshData;
        Vector<Vector<SubMesh>> allSubMeshes;
        UINT32 currentIndex = 0;

        for (auto& mesh : scene.Meshes)
        {
            Vector<SubMesh> subMeshes;
            UINT32 numIndices = (UINT32)mesh->Indices.size();

            Vector<Vector<UINT32>> indicesPerMaterial;

            // Trying to find all submeshes indices and offset
            for (UINT32 i = 0; i < scene.Materials.size(); i++)
            {
                indicesPerMaterial.push_back(Vector<UINT32>());
                if (mesh->MaterialIndex == scene.Materials[i].Index)
                {
                    for (UINT32 j = 0; j < (UINT32)mesh->Indices.size(); j++)
                    {
                        UINT32 materialIdx = (UINT32)scene.Materials[i].Index;
                        indicesPerMaterial[materialIdx].push_back(mesh->Indices[j]);
                    }
                }
            }
            for (auto key = 0; key < indicesPerMaterial.size(); key++)
            {
                if (indicesPerMaterial[key].size() == 0)
                    continue;

                UINT32 indexCount = (UINT32)indicesPerMaterial[key].size();
                subMeshes.push_back(SubMesh(currentIndex, indexCount, DOT_TRIANGLE_LIST, scene.Materials[key].Name));
                currentIndex += indexCount;
            }

            UINT32 vertexLayout = (UINT32)VertexLayout::Position;

            size_t numVertices = mesh->Positions.size();
            bool hasColors = mesh->Colors.size() == numVertices;
            bool hasNormals = mesh->Normals.size() == numVertices;
            bool hasTangents = false;

            if (hasColors)
                vertexLayout |= (UINT32)VertexLayout::Color;

            if (hasNormals)
            {
                vertexLayout |= (UINT32)VertexLayout::Normal;

                if (mesh->Tangents.size() == numVertices &&
                    mesh->Bitangents.size() == numVertices)
                {
                    vertexLayout |= (UINT32)VertexLayout::Tangent;
                    vertexLayout |= (UINT32)VertexLayout::BiTangent;
                    hasTangents = true;
                }
            }

            for (UINT32 i = 0; i < OBJECT_IMPORT_MAX_UV_LAYERS; i++)
            {
                if (mesh->Textures[i].size() == numVertices)
                {
                    if (i == 0)
                        vertexLayout |= (UINT32)VertexLayout::UV0;
                    else if (i == 1)
                        vertexLayout |= (UINT32)VertexLayout::UV1;
                }
            }

            for (auto& node : mesh->ReferencedBy)
            {
                Matrix4 worldTransform = node->WorldTransform;
                Matrix4 worldTransformIT = worldTransform.Inverse();
                worldTransformIT = worldTransformIT.Transpose();

                SPtr<RendererMeshData> meshData = RendererMeshData::Create((UINT32)numVertices, numIndices, (VertexLayout)vertexLayout);

                // Copy indices
                meshData->SetIndices(mesh->Indices.data(), numIndices * sizeof(UINT32));

                // Copy & transform positions
                UINT32 positionsSize = sizeof(Vector3) * (UINT32)numVertices;
                Vector3* transformedPositions = (Vector3*)te_allocate(positionsSize * sizeof(Vector3));

                for (UINT32 i = 0; i < (UINT32)numVertices; i++)
                {
                    transformedPositions[i] = worldTransform.MultiplyAffine((Vector3)mesh->Positions[i]);
                }

                meshData->SetPositions(transformedPositions, positionsSize);
                te_delete(transformedPositions);

                // Copy & transform normals
                if (hasNormals)
                {
                    UINT32 normalsSize = sizeof(Vector3) * (UINT32)numVertices;
                    Vector3* transformedNormals = (Vector3*)te_allocate(normalsSize * sizeof(Vector3));

                    // Copy, convert & transform tangents & bitangents
                    if (hasTangents)
                    {
                        UINT32 tangentsSize = sizeof(Vector4) * (UINT32)numVertices;
                        Vector4* transformedTangents = (Vector4*)te_allocate(tangentsSize * sizeof(Vector4));
                        Vector4* transformedBiTangents = (Vector4*)te_allocate(tangentsSize * sizeof(Vector4));

                        for (UINT32 i = 0; i < (UINT32)numVertices; i++)
                        {
                            Vector3 normal = (Vector3)mesh->Normals[i];
                            normal = worldTransformIT.MultiplyDirection(normal);
                            transformedNormals[i] = Vector3::Normalize(normal);

                            Vector3 tangent = (Vector3)mesh->Tangents[i];
                            tangent = Vector3::Normalize(worldTransformIT.MultiplyDirection(tangent));

                            Vector3 bitangent = (Vector3)mesh->Bitangents[i];
                            bitangent = worldTransformIT.MultiplyDirection(bitangent);

                            Vector3 engineBitangent = Vector3::Cross(normal, tangent);
                            float sign = Vector3::Dot(engineBitangent, bitangent);

                            bitangent = Vector3::Normalize(bitangent);

                            transformedTangents[i] = Vector4(tangent.x, tangent.y, tangent.z, sign > 0 ? 1.0f : -1.0f);
                            transformedBiTangents[i] = Vector4(bitangent.x, bitangent.y, bitangent.z, sign > 0 ? 1.0f : -1.0f);
                        }

                        meshData->SetTangents(transformedTangents, tangentsSize);
                        meshData->SetBiTangents(transformedBiTangents, tangentsSize);
                        te_delete(transformedTangents);
                        te_delete(transformedBiTangents);
                    }
                    else
                    {
                        for (UINT32 i = 0; i < (UINT32)numVertices; i++)
                        {
                            transformedNormals[i] = Vector3::Normalize(worldTransformIT.MultiplyDirection((Vector3)mesh->Normals[i]));
                        }
                    }

                    meshData->SetNormals(transformedNormals, normalsSize);
                    te_delete(transformedNormals);
                }

                // Copy colors
                if (hasColors)
                {
                    meshData->SetColors(mesh->Colors.data(), sizeof(Vector4) * (UINT32)numVertices);
                }

                // Copy UV
                int writeUVIDx = 0;
                for (auto& uvLayer : mesh->Textures)
                {
                    if (uvLayer.size() == numVertices)
                    {
                        UINT32 size = sizeof(Vector2) * (UINT32)numVertices;
                        Vector2* transformedUV = (Vector2*)te_allocate(size * sizeof(Vector2));

                        UINT32 i = 0;
                        for (auto& uv : uvLayer)
                        {
                            transformedUV[i] = uv;
                            transformedUV[i].y = 1.0f - uv.y;
                            i++;
                        }

                        if (writeUVIDx == 0)
                            meshData->SetUV0(transformedUV, size);
                        else if (writeUVIDx == 1)
                            meshData->SetUV1(transformedUV, size);

                        te_delete(transformedUV);

                        writeUVIDx++;
                    }
                }

                allMeshData.push_back(meshData->GetData());
                allSubMeshes.push_back(subMeshes);
            }
        }

        if (allMeshData.size() > 1)
        {
            return RendererMeshData::Create(MeshData::Combine(allMeshData, allSubMeshes, outputSubMeshes));
        }
        else if (allMeshData.size() == 1)
        {
            outputSubMeshes = allSubMeshes[0];
            return RendererMeshData::Create(allMeshData[0]);
        }

        return nullptr;
    }

    AssimpImportNode* ObjectImporter::CreateImportNode(const AssimpImportOptions& options, AssimpImportScene& scene, aiNode* assimpNode, AssimpImportNode* parent)
    {
        AssimpImportNode* node = te_new<AssimpImportNode>();
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

    Matrix4 ObjectImporter::ConvertToNativeType(const aiMatrix4x4& matrix)
    {
        return Matrix4(
            (float)matrix.a1, (float)matrix.a2, (float)matrix.a3, (float)matrix.a4,
            (float)matrix.b1, (float)matrix.b2, (float)matrix.b3, (float)matrix.b4,
            (float)matrix.c1, (float)matrix.c2, (float)matrix.c3, (float)matrix.c4,
            (float)matrix.d1, (float)matrix.d2, (float)matrix.d3, (float)matrix.d4
        );
    }

    Vector4 ObjectImporter::ConvertToNativeType(const aiColor4D& color)
    {
        return Vector4(
            (float)color.r,
            (float)color.g,
            (float)color.b,
            (float)color.a
        );
    }

    Vector3 ObjectImporter::ConvertToNativeType(const aiVector3D& vector)
    {
        return Vector3((float)vector.x, (float)vector.y, (float)vector.z);
    }

    Vector2 ObjectImporter::ConvertToNativeType(const aiVector2D& vector)
    {
        return Vector2((float)vector.x, (float)vector.y);
    }
}
