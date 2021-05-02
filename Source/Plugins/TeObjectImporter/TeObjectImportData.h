#pragma once

#include "TeObjectImporterPrerequisites.h"
#include "Renderer/TeRendererMeshData.h"
#include "Image/TeColor.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2.h"
#include "Math/TeMatrix4.h"
#include "Material/TeMaterial.h"
#include "Animation/TeAnimationCurve.h"
#include "Animation/TeAnimationClip.h"
#include "RenderAPI/TeSubMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace te
{
    /**	Options that control assimp import */
    struct AssimpImportOptions
    {
        bool ImportNormals = true;
        bool ImportTangents = true;
        bool ImportColors = true;
        bool ImportSkin = false;
        bool ImportAnimations = true;
        bool ImportMaterials = true;
        bool ImportBlendShapes = true;
        bool ForceGenNormals = false;
        bool GenSmoothNormals = false;
        float AnimSampleRate = 1.0f / 60.0f;
        bool AnimResample = false;
        bool ReduceKeyframes = true;
        String FilePath;
    };

    /**	Represents a single node in the assimp hierarchy. */
    struct AssimpImportNode
    {
        AssimpImportNode() = default;
        ~AssimpImportNode();

        Matrix4 LocalTransform = Matrix4::IDENTITY;
        Matrix4 WorldTransform = Matrix4::IDENTITY;
        String Name;
        aiNode* Node = nullptr;

        Vector<AssimpImportNode*> Children;
    };

    /**	Contains data about a single bone in a skinned mesh. */
    struct AssimpBone
    {
        AssimpImportNode* Node;
        Matrix4 LocalTransform = Matrix4::IDENTITY;
        Matrix4 InvBindPose = Matrix4::IDENTITY;
    };

    /** Contains a set of bone weights and indices for a single vertex, used in a skinned mesh. */
    struct AssimpBoneInfluence
    {
        AssimpBoneInfluence()
        {
            for (UINT32 i = 0; i < OBJECT_IMPORT_MAX_BONE_INFLUENCES; i++)
            {
                Weights[i] = 0.0f;
                Indices[i] = -1;
            }
        }

        float Weights[OBJECT_IMPORT_MAX_BONE_INFLUENCES];
        INT32 Indices[OBJECT_IMPORT_MAX_BONE_INFLUENCES]; // Bone Index
    };

    /**	Animation curves required to animate a single bone. */
    struct AssimpBoneAnimation
    {
        AssimpImportNode* Node;

        TAnimationCurve<Vector3> Translation;
        TAnimationCurve<Quaternion> Rotation;
        TAnimationCurve<Vector3> Scale;
    };

    /** All information required for creating an animation clip. */
    struct AssimpAnimationClipData
    {
        AssimpAnimationClipData(const String& name, bool isAdditive, float sampleRate, 
            const SPtr<AnimationCurves>& curves, const SPtr<RootMotion>& rootMotion)
            : Name(name)
            , IsAdditive(isAdditive)
            , SampleRate(sampleRate)
            , Curves(curves)
            , RootMot(rootMotion)
        { }

        String Name;
        bool IsAdditive;
        float SampleRate;
        SPtr<AnimationCurves> Curves;
        SPtr<RootMotion> RootMot;
    };

    /** Animation clip containing a set of bone or blend shape animations. */
    struct AssimpAnimationClip
    {
        String Name;
        float Start;
        float End;
        float SampleRate;

        Vector<AssimpBoneAnimation> BoneAnimations;
    };
    
    /**	Imported mesh data. */
    struct AssimpImportMesh
    {
        aiMesh* AssimpMesh;

        Vector<UINT32> Indices;
        Vector<Vector3> Positions;
        Vector<Vector3> Normals;
        Vector<Vector3> Tangents;
        Vector<Vector3> Bitangents;
        Vector<Vector4> Colors;
        Vector<Vector2> Textures[OBJECT_IMPORT_MAX_UV_LAYERS];
        UINT32 MaterialIndex;

        Vector<AssimpBoneInfluence> BoneInfluences;
        Vector<AssimpBone> Bones;

        SPtr<MeshData> Data;
        Vector<SubMesh> SubMeshes;

        Vector<AssimpImportNode*> ReferencedBy;
    };

    struct AssimpImportMaterial
    {
        UINT32 Index;
        String Name;
        MaterialProperties MatProperties;
        MaterialTextures   MatTextures;
    };

    /**	Scene information used and modified during FBX import. */
    struct AssimpImportScene
    {
        AssimpImportScene() = default;
        ~AssimpImportScene();

        Vector<AssimpImportMesh*> Meshes;
        Vector<AssimpImportMaterial> Materials;

        AssimpImportNode* RootNode = nullptr;

        UnorderedMap<aiNode*, AssimpImportNode*> NodeMap;
        UnorderedMap<String, AssimpImportNode*> NodeNameMap;
        UnorderedMap<aiMesh*, UINT32> MeshMap;

        Vector<AssimpAnimationClip> Clips;
    };    
}
