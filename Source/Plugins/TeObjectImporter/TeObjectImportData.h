#pragma once

#include "TeObjectImporterPrerequisites.h"
#include "Renderer/TeRendererMeshData.h"
#include "Utility/TeColor.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2.h"
#include "Math/TeMatrix4.h"
#include "Scene/TeTransform.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace te
{
    /**	Options that control FBX import */
    struct AssimpImportOptions
    {
        bool ImportNormals = true;
        bool ImportTangents = true;
        float ImportScale = 0.01f;
    };

    struct AssimpImportNode
    {
        AssimpImportNode() = default;
        ~AssimpImportNode();

        Matrix4 LocalTransform;
        Matrix4 WorldTransform;
        String Name;
        aiNode* Node;

        Vector<AssimpImportNode*> Children;
    };
    
    struct AssimpImportMesh
    {
        aiMesh* AssimpMesh;

        Vector<UINT32> Indices;
        Vector<Vector3> Positions;
        Vector<Vector3> Normals;
        Vector<Vector3> Tangents;
        Vector<Vector3> Bitangents;
        Vector<RGBA> Colors;
        Vector<Vector2> Textures;

        SPtr<MeshData> MeshData;
        Vector<SubMesh> SubMeshes;

        Vector<AssimpImportNode*> ReferencedBy;
    };

    struct AssimpImportScene
    {
        AssimpImportScene() = default;
        ~AssimpImportScene();

        Vector<AssimpImportMesh*> Meshes;

        AssimpImportNode* RootNode = nullptr;

        UnorderedMap<aiNode*, AssimpImportNode*> NodeMap;
        UnorderedMap<aiMesh*, UINT32> MeshMap;
    };
}
