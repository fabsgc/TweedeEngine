#include "TeObjectImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Mesh/TeMesh.h"
#include "Mesh/TeMeshData.h"
#include "Image/TeColor.h"
#include "Animation/TeSkeleton.h"
#include "Animation/TeAnimationUtility.h"

namespace te
{
    ObjectImporter::ObjectImporter()
        : BaseImporter()
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

        SetMeshImportOptions(filePath, *meshImportOptions);

        Vector<AssimpAnimationClipData> dummy;
        SPtr<RendererMeshData> rendererMeshData = ImportMeshData(filePath, importOptions, desc.SubMeshes, dummy, desc.MeshSkeleton);

        if (rendererMeshData)
        {
            SPtr<Mesh> mesh = Mesh::_createPtr(rendererMeshData->GetData(), desc);
            mesh->SetName(filePath);
            mesh->SetPath(filePath);

            return mesh;
        }
        
        return nullptr;
    }

    Vector<SubResourceRaw> ObjectImporter::ImportAll(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        MESH_DESC desc;
        MeshImportOptions* meshImportOptions = const_cast<MeshImportOptions*>
            (static_cast<const MeshImportOptions*>(importOptions.get()));

        SetMeshImportOptions(filePath, *meshImportOptions);

        Vector<AssimpAnimationClipData> animationClips;
        SPtr<RendererMeshData> rendererMeshData = ImportMeshData(filePath, importOptions, desc.SubMeshes, animationClips, desc.MeshSkeleton);

        Vector<SubResourceRaw> output;
        if (rendererMeshData)
        {
            SPtr<Mesh> mesh = Mesh::_createPtr(rendererMeshData->GetData(), desc);
            mesh->SetName(filePath);
            mesh->SetPath(filePath);

            if (mesh != nullptr)
            {
                output.push_back({ u8"primary", mesh });

                Vector<ImportedAnimationEvents> events = meshImportOptions->AnimationEvents;
                for (auto& entry : animationClips)
                {
                    SPtr<AnimationClip> clip = AnimationClip::_createPtr(entry.Curves, entry.SampleRate, entry.RootMot);
                    clip->SetName(entry.Name);

                    for (auto& eventsEntry : events)
                    {
                        if (entry.Name == eventsEntry.Name)
                        {
                            clip->SetEvents(eventsEntry.Events);
                            break;
                        }
                    }

                    output.push_back({ entry.Name, clip });
                }
            }
        }

        return output;
    }

    SPtr<RendererMeshData> ObjectImporter::ImportMeshData(const String& filePath, SPtr<const ImportOptions> importOptions, Vector<SubMesh>& subMeshes, 
        Vector<AssimpAnimationClipData>& animation, SPtr<Skeleton>& skeleton)
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
            aiProcess_SortByPType |
            aiProcess_RemoveRedundantMaterials;

        if (meshImportOptions->FplitUV)
            assimpFlags |= aiProcess_FlipUVs;

        if (meshImportOptions->FlipWinding)
            assimpFlags |= aiProcess_FlipWindingOrder;

        if (meshImportOptions->LeftHanded)
            assimpFlags |= aiProcess_MakeLeftHanded;

        if (meshImportOptions->ImportAnimation)
            assimpFlags |= aiProcess_LimitBoneWeights;

        aiScene* scene = const_cast<aiScene*>(importer.ReadFile(filePath.c_str(), assimpFlags));

        if (!scene)
        {
            TE_DEBUG("Failed to load object '" + filePath + "' : " + importer.GetErrorString());
            return nullptr;
        }

        AssimpImportOptions assimpImportOptions;
        assimpImportOptions.ImportNormals      = meshImportOptions->ImportNormals;
        assimpImportOptions.ImportTangents     = meshImportOptions->ImportTangents;
        assimpImportOptions.ImportSkin         = meshImportOptions->ImportSkin;
        assimpImportOptions.ImportBlendShapes  = meshImportOptions->ImportBlendShapes;
        assimpImportOptions.ImportAnimation    = meshImportOptions->ImportAnimation;
        assimpImportOptions.ImportMaterials    = meshImportOptions->ImportMaterials;
        assimpImportOptions.ScaleSystemUnit    = meshImportOptions->ScaleSystemUnit;
        assimpImportOptions.ScaleFactor        = meshImportOptions->ScaleFactor;

        ParseScene(scene, assimpImportOptions, importedScene);

        if (assimpImportOptions.ImportSkin)
            ImportSkin(importedScene, assimpImportOptions);

        if (assimpImportOptions.ImportAnimation)
            ImportAnimations(scene, assimpImportOptions, importedScene);

        SPtr<RendererMeshData> rendererMeshData = GenerateMeshData(importedScene, assimpImportOptions, subMeshes);

        skeleton = CreateSkeleton(importedScene, subMeshes.size() > 1);

        // Import animation clips
        if (!importedScene.Clips.empty())
        {
            const Vector<AnimationSplitInfo>& splits = meshImportOptions->AnimationSplits;
            ConvertAnimations(importedScene.Clips, splits, skeleton, meshImportOptions->ImportRootMotion, animation);
        }

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
            aiMaterial* aiMat = nullptr;
            aiString matName;

            for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            {
                aiMat = scene->mMaterials[i];
                if (!aiMat) continue;

                aiMat->Get(AI_MATKEY_NAME, matName);

                material.Index = i;
                material.Name = matName.C_Str();

                if (options.ImportMaterials)
                {
                    auto BindTexture = [this](aiMaterial* aiMat, aiTextureType textureType, String& path, bool& use)
                    {
                        aiString aiPath;
                        int textureIndex = 0;
                        if (aiMat->GetTexture(textureType, textureIndex, &aiPath) == AI_SUCCESS)
                        {
                            path = aiPath.C_Str();
                            use = true;
                        }
                    };

                    BindTexture(aiMat, aiTextureType_DIFFUSE, material.MatTextures.DiffuseMap, material.MatProperties.UseDiffuseMap);
                    BindTexture(aiMat, aiTextureType_EMISSIVE, material.MatTextures.EmissiveMap, material.MatProperties.UseEmissiveMap);
                    BindTexture(aiMat, aiTextureType_NORMALS, material.MatTextures.NormalMap, material.MatProperties.UseNormalMap);
                    BindTexture(aiMat, aiTextureType_SPECULAR, material.MatTextures.SpecularMap, material.MatProperties.UseSpecularMap);
                    BindTexture(aiMat, aiTextureType_REFLECTION, material.MatTextures.ReflectionMap, material.MatProperties.UseReflectionMap);
                    BindTexture(aiMat, aiTextureType_OPACITY, material.MatTextures.TransparencyMap, material.MatProperties.UseTransparencyMap);
                    BindTexture(aiMat, aiTextureType_DISPLACEMENT, material.MatTextures.NormalMap, material.MatProperties.UseNormalMap);
                    BindTexture(aiMat, aiTextureType_DISPLACEMENT, material.MatTextures.BumpMap, material.MatProperties.UseBumpMap);

                    // TODO parallax, occlusion and environment map can't be set here

                    aiColor3D ambientColor;
                    aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
                    material.MatProperties.Ambient = ConvertToNativeType(ambientColor);

                    aiColor3D diffuseColor;
                    aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
                    material.MatProperties.Diffuse = ConvertToNativeType(diffuseColor);

                    aiColor3D specularColor;
                    aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
                    material.MatProperties.Ambient = ConvertToNativeType(ambientColor);

                    aiColor3D emissiveColor;
                    aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
                    material.MatProperties.Emissive = ConvertToNativeType(emissiveColor);

                    aiMat->Get(AI_MATKEY_SHININESS, material.MatProperties.SpecularPower);
                    aiMat->Get(AI_MATKEY_SHININESS_STRENGTH, material.MatProperties.SpecularStrength);
                    aiMat->Get(AI_MATKEY_OPACITY, material.MatProperties.Transparency);
                    aiMat->Get(AI_MATKEY_REFRACTI, material.MatProperties.IndexOfRefraction);
                    aiMat->Get(AI_MATKEY_BUMPSCALING, material.MatProperties.BumpScale);
                    aiMat->Get(AI_MATKEY_REFLECTIVITY, material.MatProperties.Reflection);
                }

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
                importMesh->Positions[i] *= options.ScaleFactor;

            if (mesh->HasVertexColors(i) && options.ImportColors)
                importMesh->Colors[i] = ConvertToNativeType(*mesh->mColors[i]);
            else
                importMesh->Colors[i] = Color::LightGray.GetAsVector4();
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

    void ObjectImporter::ImportSkin(AssimpImportScene& scene, const AssimpImportOptions& options)
    {
        for (auto& mesh : scene.Meshes)
        {
            aiMesh* assimpMesh = mesh->AssimpMesh;

            if (assimpMesh->mNumBones == 0)
                continue;

            ImportSkin(scene, assimpMesh, *mesh, options);
        }
    }

    void ObjectImporter::ImportSkin(AssimpImportScene& scene, aiMesh* assimpMesh, AssimpImportMesh& mesh, const AssimpImportOptions& options)
    {
        Vector<AssimpBoneInfluence>& influences = mesh.BoneInfluences;
        influences.resize(mesh.Positions.size());

        UINT32 boneCount = (UINT32)assimpMesh->mNumBones;
        for (UINT32 i = 0; i < boneCount; i++)
        {
            String boneName = assimpMesh->mBones[i]->mName.data;
            auto iterNode = scene.NodeNameMap.find(boneName);

            if (iterNode == scene.NodeNameMap.end())
                continue;

            mesh.Bones.push_back(AssimpBone());

            AssimpBone& bone = mesh.Bones.back();
            bone.Node = iterNode->second;

            if (mesh.ReferencedBy.size() > 1)
            {
                // Note: If this becomes a relevant issue (unlikely), then I will have to duplicate skeleton bones for
                // each such mesh, since they will all require their own bind poses. Animation curves will also need to be
                // handled specially (likely by allowing them to be applied to multiple bones at once). The other option is
                // not to bake the node transform into mesh vertices and handle it on a Scene Object level.
                TE_DEBUG("Skinned mesh has multiple different instances. This is not supported.");
            }

            AssimpImportNode* parentNode = mesh.ReferencedBy[0];

            bone.LocalTransform = bone.Node->LocalTransform; // TODO Matrix4 or Transform ?
            bone.BindPose = ConvertToNativeType(assimpMesh->mBones[i]->mOffsetMatrix);

            // Undo the transform we baked into the mesh
            bone.BindPose = bone.BindPose * (parentNode->WorldTransform).InverseAffine();

            INT32 numVertices = (INT32)influences.size();

            for (UINT32 j = 0; j < assimpMesh->mBones[i]->mNumWeights; j++)
            {
                INT32 vertexIndex = assimpMesh->mBones[i]->mWeights[j].mVertexId;
                float weight = assimpMesh->mBones[i]->mWeights[j].mWeight;

                for (INT32 k = 0; k < OBJECT_IMPORT_MAX_BONE_INFLUENCES; k++)
                {
                    if (vertexIndex < 0 || vertexIndex >= numVertices)
                        continue;

                    if (weight >= influences[vertexIndex].Weights[k])
                    {
                        for (INT32 l = OBJECT_IMPORT_MAX_BONE_INFLUENCES - 2; l >= k; l--)
                        {
                            influences[vertexIndex].Weights[l + 1] = influences[vertexIndex].Weights[l];
                            influences[vertexIndex].Indices[l + 1] = influences[vertexIndex].Indices[l];
                        }

                        influences[vertexIndex].Weights[k] = weight;
                        influences[vertexIndex].Indices[k] = i;
                        break;
                    }
                }
            }
        }

        if (mesh.Bones.empty())
            mesh.BoneInfluences.clear();

        UINT32 numBones = (UINT32)mesh.Bones.size();
        if (numBones > 256)
        {
            TE_DEBUG("A maximum of 256 bones per skeleton are supported. Imported skeleton has " + ToString(numBones) + " bones.");
        }

        // Normalize weights
        UINT32 numInfluences = (UINT32)mesh.BoneInfluences.size();
        for (UINT32 i = 0; i < numInfluences; i++)
        {
            float sum = 0.0f;
            for (UINT32 j = 0; j < OBJECT_IMPORT_MAX_BONE_INFLUENCES; j++)
                sum += influences[i].Weights[j];

            float invSum = 1.0f / sum;
            for (UINT32 j = 0; j < OBJECT_IMPORT_MAX_BONE_INFLUENCES; j++)
                influences[i].Weights[j] *= invSum;
        }
    }

    void ObjectImporter::ImportAnimations(aiScene* scene, AssimpImportOptions& importOptions, AssimpImportScene& importScene)
    {
        for (UINT32 i = 0; i < scene->mNumAnimations; i++)
        {
            aiAnimation* assimAnimation = scene->mAnimations[i];
            importScene.Clips.push_back(AssimpAnimationClip());

            AssimpAnimationClip& clip = importScene.Clips.back();
            clip.Name = (assimAnimation->mName.length > 0) ? assimAnimation->mName.C_Str() : "Animation " + ToString(i);
            clip.Start = 0.0f;
            clip.End = (float)assimAnimation->mDuration;
            clip.SampleRate = assimAnimation->mTicksPerSecond != 0.0 ? (float)assimAnimation->mTicksPerSecond : 25.0f;

            for (UINT32 j = 0; j < static_cast<UINT32>(assimAnimation->mNumChannels); j++)
            {
                aiNodeAnim* assimpChannel = assimAnimation->mChannels[j];
                Vector<TKeyframe<Vector3>> positions;
                Vector<TKeyframe<Quaternion>> rotations;
                Vector<TKeyframe<Vector3>> scalings;

                clip.BoneAnimations.push_back(AssimpBoneAnimation());
                AssimpBoneAnimation& boneAnim = clip.BoneAnimations.back();
                boneAnim.Node = importScene.NodeNameMap[assimpChannel->mNodeName.C_Str()];

                // Position keys
                if (assimpChannel->mNumPositionKeys > 0)
                {
                    for (UINT32 k = 0; k < static_cast<UINT32>(assimpChannel->mNumPositionKeys); k++)
                    {
                        positions.push_back(TKeyframe<Vector3>());
                        TKeyframe<Vector3>& keyFrame = positions.back();

                        keyFrame.TimeInSpline = (float)assimpChannel->mPositionKeys[k].mTime;
                        keyFrame.Value = ConvertToNativeType(assimpChannel->mPositionKeys[k].mValue);
                    }
                }
                else
                {
                    positions.push_back(TKeyframe<Vector3>());
                    TKeyframe<Vector3>& keyFrame = positions.back();

                    keyFrame.TimeInSpline = 0.0f;
                    keyFrame.Value = Vector3();
                }

                // Rotation keys
                if (assimpChannel->mNumRotationKeys > 0)
                {
                    for (UINT32 k = 0; k < static_cast<UINT32>(assimpChannel->mNumRotationKeys); k++)
                    {
                        rotations.push_back(TKeyframe<Quaternion>());
                        TKeyframe<Quaternion>& keyFrame = rotations.back();

                        keyFrame.TimeInSpline = (float)assimpChannel->mRotationKeys[k].mTime;
                        keyFrame.Value = ConvertToNativeType(assimpChannel->mRotationKeys[k].mValue);
                    }
                }

                else
                {
                    rotations.push_back(TKeyframe<Quaternion>());
                    TKeyframe<Quaternion>& keyFrame = rotations.back();

                    keyFrame.TimeInSpline = 0.0f;
                    keyFrame.Value = Quaternion();
                }

                // Scaling keys
                if (assimpChannel->mNumScalingKeys > 0)
                {
                    for (UINT32 k = 0; k < static_cast<UINT32>(assimpChannel->mNumScalingKeys); k++)
                    {
                        scalings.push_back(TKeyframe<Vector3>());
                        TKeyframe<Vector3>& keyFrame = scalings.back();

                        keyFrame.TimeInSpline = (float)assimpChannel->mScalingKeys[k].mTime;
                        keyFrame.Value = ConvertToNativeType(assimpChannel->mScalingKeys[k].mValue);
                    }
                }
                else
                {
                    scalings.push_back(TKeyframe<Vector3>());
                    TKeyframe<Vector3>& keyFrame = scalings.back();

                    keyFrame.TimeInSpline = 0.0f;
                    keyFrame.Value = Vector3();
                }

                boneAnim.Translation = TAnimationCurve<Vector3>(positions);
                boneAnim.Rotation = TAnimationCurve<Quaternion>(rotations);
                boneAnim.Scale = TAnimationCurve<Vector3>(scalings);
            }
        }
    }

    SPtr<Skeleton> ObjectImporter::CreateSkeleton(const AssimpImportScene& scene, bool sharedRoot)
    {
        Vector<BONE_DESC> allBones;
        UnorderedMap<AssimpImportNode*, UINT32> boneMap;

        Vector3 position;
        Quaternion rotation;
        Vector3 scale;

        for (auto& mesh : scene.Meshes)
        {
            // Create bones
            for (auto& assimpBone : mesh->Bones)
            {
                UINT32 boneIdx = (UINT32)allBones.size();

                auto iterFind = boneMap.find(assimpBone.Node);
                if (iterFind != boneMap.end())
                    continue; // Duplicate

                boneMap[assimpBone.Node] = boneIdx;

                allBones.push_back(BONE_DESC());
                BONE_DESC& bone = allBones.back();

                assimpBone.LocalTransform.Decomposition(position, rotation, scale); // TODO don't like that, slow
                bone.Name = assimpBone.Node->Name;
                bone.LocalTfrm = Transform(position, rotation, scale);
                bone.InvBindPose = assimpBone.BindPose;
            }
        }

        // Generate skeleton
        if (allBones.size() > 0)
        {
            // Find bone parents
            UINT32 numProcessedBones = 0;

            // Generate common root bone for all meshes
            UINT32 rootBoneIdx = (UINT32)-1;
            if (sharedRoot)
            {
                rootBoneIdx = (UINT32)allBones.size();

                allBones.push_back(BONE_DESC());
                BONE_DESC& bone = allBones.back();

                bone.Name = "MultiMeshRoot";
                bone.LocalTfrm = Transform();
                bone.InvBindPose = Matrix4::IDENTITY;
                bone.Parent = (UINT32)-1;

                numProcessedBones++;
            }

            Stack<std::pair<AssimpImportNode*, UINT32>> todo;
            todo.push({ scene.RootNode, rootBoneIdx });

            while (!todo.empty())
            {
                auto entry = todo.top();
                todo.pop();

                AssimpImportNode* node = entry.first;
                UINT32 parentBoneIdx = entry.second;

                auto boneIter = boneMap.find(node);
                if (boneIter != boneMap.end())
                {
                    UINT32 boneIdx = boneIter->second;
                    allBones[boneIdx].Parent = parentBoneIdx;

                    parentBoneIdx = boneIdx;
                    numProcessedBones++;
                }
                else
                {
                    // Node is not a bone, but it still needs to be part of the hierarchy. It wont be animated, nor will
                    // it directly influence any vertices, but its transform must be applied to any child bones.
                    UINT32 boneIdx = (UINT32)allBones.size();

                    allBones.push_back(BONE_DESC());
                    BONE_DESC& bone = allBones.back();
                    node->LocalTransform.Decomposition(position, rotation, scale); // TODO don't like that, slow

                    bone.Name = node->Name;
                    bone.LocalTfrm = Transform(position, rotation, scale);
                    bone.InvBindPose = Matrix4::IDENTITY;
                    bone.Parent = parentBoneIdx;

                    parentBoneIdx = boneIdx;
                    numProcessedBones++;
                }

                for (auto& child : node->Children)
                    todo.push({ child, parentBoneIdx });
            }

            UINT32 numAllBones = (UINT32)allBones.size();
            if (numProcessedBones == numAllBones)
                return Skeleton::Create(allBones.data(), numAllBones);

            TE_DEBUG("Not all bones were found in the node hierarchy. Skeleton invalid.");
        }

        return nullptr;
    }

    void ObjectImporter::ConvertAnimations(const Vector<AssimpAnimationClip>& clips, const Vector<AnimationSplitInfo>& splits,
        const SPtr<Skeleton>& skeleton, bool importRootMotion, Vector<AssimpAnimationClipData>& output)
    {
        UnorderedSet<String> names;

        String rootBoneName;
        if (skeleton == nullptr)
        {
            importRootMotion = false;
        }
        else
        {
            UINT32 rootBoneIdx = skeleton->GetRootBoneIndex();
            if (rootBoneIdx == (UINT32)-1)
                importRootMotion = false;
            else
                rootBoneName = skeleton->GetBoneInfo(rootBoneIdx).Name;
        }

        for (auto& clip : clips)
        {
            SPtr<AnimationCurves> curves = te_shared_ptr_new<AnimationCurves>();
            SPtr<RootMotion> rootMotion;

            // Find offset so animations start at time 0
            float animStart = std::numeric_limits<float>::infinity();

            for (auto& bone : clip.BoneAnimations)
            {
                if (bone.Translation.GetNumKeyFrames() > 0)
                    animStart = std::min(bone.Translation.GetKeyFrame(0).TimeInSpline, animStart);

                if (bone.Rotation.GetNumKeyFrames() > 0)
                    animStart = std::min(bone.Rotation.GetKeyFrame(0).TimeInSpline, animStart);

                if (bone.Scale.GetNumKeyFrames() > 0)
                    animStart = std::min(bone.Scale.GetKeyFrame(0).TimeInSpline, animStart);
            }

            if (animStart != 0.0f && animStart != std::numeric_limits<float>::infinity())
            {
                for (auto& bone : clip.BoneAnimations)
                {
                    TAnimationCurve<Vector3> translation = AnimationUtility::OffsetCurve(bone.Translation, -animStart);
                    TAnimationCurve<Quaternion> rotation = AnimationUtility::OffsetCurve(bone.Rotation, -animStart);
                    TAnimationCurve<Vector3> scale = AnimationUtility::OffsetCurve(bone.Scale, -animStart);

                    if (importRootMotion && bone.Node->Name == rootBoneName)
                    {
                        rootMotion = te_shared_ptr_new<RootMotion>(translation, rotation);
                    }
                    else
                    {
                        curves->Position.push_back({ bone.Node->Name, (UINT32)AnimationCurveFlag::ImportedCurve, translation });
                        curves->Rotation.push_back({ bone.Node->Name, (UINT32)AnimationCurveFlag::ImportedCurve, rotation });
                        curves->Scale.push_back({ bone.Node->Name, (UINT32)AnimationCurveFlag::ImportedCurve, scale });
                    }
                }
            }
            else
            {
                for (auto& bone : clip.BoneAnimations)
                {
                    if (importRootMotion && bone.Node->Name == rootBoneName)
                        rootMotion = te_shared_ptr_new<RootMotion>(bone.Translation, bone.Rotation);
                    else
                    {
                        curves->Position.push_back({ bone.Node->Name, (UINT32)AnimationCurveFlag::ImportedCurve, bone.Translation });
                        curves->Rotation.push_back({ bone.Node->Name, (UINT32)AnimationCurveFlag::ImportedCurve, bone.Rotation });
                        curves->Scale.push_back({ bone.Node->Name, (UINT32)AnimationCurveFlag::ImportedCurve, bone.Scale });
                    }
                }
            }

            // Search for a unique name
            String name = clip.Name;
            UINT32 attemptIdx = 0;
            while (names.find(name) != names.end())
            {
                name = clip.Name + "_" + ToString(attemptIdx);
                attemptIdx++;
            }

            names.insert(name);
            output.push_back(AssimpAnimationClipData(name, clip.SampleRate, curves, rootMotion));
        }
    }

    SPtr<RendererMeshData> ObjectImporter::GenerateMeshData(AssimpImportScene& scene, AssimpImportOptions& options, Vector<SubMesh>& outputSubMeshes)
    {
        Vector<SPtr<MeshData>> allMeshData;
        Vector<Vector<SubMesh>> allSubMeshes;
        UINT32 currentIndex = 0;
        UINT32 boneIndexOffset = 0;

        // Generate unique indices for all the bones. This is mirrored in createSkeleton().
        UnorderedMap<AssimpImportNode*, UINT32> boneMap;
        for (auto& mesh : scene.Meshes)
        {
            // Create bones
            for (auto& assimpBone : mesh->Bones)
            {
                UINT32 boneIdx = (UINT32)boneMap.size();

                auto iterFind = boneMap.find(assimpBone.Node);
                if (iterFind != boneMap.end())
                    continue; // Duplicate

                boneMap[assimpBone.Node] = boneIdx;
            }
        }

        for (auto& mesh : scene.Meshes)
        {
            Vector<SubMesh> subMeshes;
            UINT32 numIndices = (UINT32)mesh->Indices.size();

            Vector<Vector<UINT32>> indicesPerMaterial;

            // Trying to find all submeshes indices and offset
            for (UINT32 i = 0; i < (UINT32)scene.Materials.size(); i++)
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
            for (UINT32 key = 0; key < (UINT32)indicesPerMaterial.size(); key++)
            {
                if (indicesPerMaterial[key].size() == 0)
                    continue;

                UINT32 indexCount = (UINT32)indicesPerMaterial[key].size();
                SubMesh subMesh(currentIndex, indexCount, DOT_TRIANGLE_LIST, scene.Materials[key].Name, "SubMesh " + ToString(key));
                
                if (options.ImportMaterials)
                {
                    subMesh.MatProperties = scene.Materials[key].MatProperties;
                    subMesh.MatTextures = scene.Materials[key].MatTextures;
                }

                subMeshes.push_back(subMesh);
                currentIndex += indexCount;
            }

            UINT32 vertexLayout = (UINT32)VertexLayout::Position;

            size_t numVertices = mesh->Positions.size();
            bool hasColors = mesh->Colors.size() == numVertices;
            bool hasNormals = mesh->Normals.size() == numVertices;
            bool hasBoneInfluences = mesh->BoneInfluences.size() == numVertices;
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

            if (hasBoneInfluences)
                vertexLayout |= (UINT32)VertexLayout::BoneWeights;

            for (UINT32 i = 0; i < OBJECT_IMPORT_MAX_UV_LAYERS; i++)
            {
                if (mesh->Textures[i].size() == numVertices)
                {
                    if (i == 0)
                        vertexLayout |= (UINT32)VertexLayout::UV0;
                    /*else if (i == 1)
                        vertexLayout |= (UINT32)VertexLayout::UV1;*/
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
                        /*else if (writeUVIDx == 1)
                            meshData->SetUV1(transformedUV, size);*/

                        te_delete(transformedUV);

                        writeUVIDx++;
                    }
                }

                // Copy bone influences & remap bone indices
                if (hasBoneInfluences)
                {
                    UINT32 bufferSize = sizeof(BoneWeight) * (UINT32)numVertices;
                    BoneWeight* weights = (BoneWeight*)te_allocate(bufferSize * sizeof(BoneWeight));
                    for (UINT32 i = 0; i < (UINT32)numVertices; i++)
                    {
                        int* indices[] = { &weights[i].Index0, &weights[i].Index1, &weights[i].Index2, &weights[i].Index3 };
                        float* amounts[] = { &weights[i].Weight0, &weights[i].Weight1, &weights[i].Weight2, &weights[i].Weight3 };

                        for (UINT32 j = 0; j < 4; j++)
                        {
                            int boneIdx = mesh->BoneInfluences[i].Indices[j];
                            if (boneIdx != -1)
                            {
                                AssimpImportNode* boneNode = mesh->Bones[boneIdx].Node;

                                auto iterFind = boneMap.find(boneNode);
                                if (iterFind != boneMap.end())
                                    *indices[j] = iterFind->second;
                                else
                                    *indices[j] = -1;
                            }
                            else
                            {
                                *indices[j] = boneIdx;
                            }

                            *amounts[j] = mesh->BoneInfluences[i].Weights[j];
                        }
                    }

                    meshData->SetBoneWeights(weights, bufferSize);
                    te_deallocate(weights);
                }

                allMeshData.push_back(meshData->GetData());
                allSubMeshes.push_back(subMeshes);
            }

            UINT32 numBones = (UINT32)mesh->Bones.size();
            boneIndexOffset += numBones;
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
        node->Node = assimpNode;

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
        scene.NodeNameMap.insert(std::make_pair(assimpNode->mName.C_Str(), node));

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

    Color ObjectImporter::ConvertToNativeType(const aiColor3D& color)
    {
        return Color((float)color.r, (float)color.g, (float)color.b, 1.0f);
    }

    Quaternion ObjectImporter::ConvertToNativeType(const aiQuaternion& quaternion)
    {
        return Quaternion((float)quaternion.x, (float)quaternion.y, (float)quaternion.z, (float)quaternion.w);
    }

    void ObjectImporter::SetMeshImportOptions(const String& filePath, MeshImportOptions& meshImportOptions)
    {
        String extension = Util::GetFileExtension(filePath);
        std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
        if (extension == ".fbx")
        {
            meshImportOptions.ScaleSystemUnit = true;
            meshImportOptions.ScaleFactor = 0.01f;
        }
    }
}
