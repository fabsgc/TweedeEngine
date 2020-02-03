#pragma once

#include "TeObjectImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "Renderer/TeRendererMeshData.h"
#include "TeObjectImportData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace te
{
    class ObjectImporter : public BaseImporter
    {
    public:
        ObjectImporter();
        virtual ~ObjectImporter();

        /** @copydoc BasicImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc BasicImporter::Import */
        SPtr<Resource> Import(const String& filePath, const SPtr<const ImportOptions> importOptions) override;

        /** @copydoc BasicImporter::CreateImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;

    private:
        /** Reads the object file and outputs mesh data from the read file. Sub-mesh information will be output in @p subMeshes. */
        SPtr<RendererMeshData> ImportMeshData(const String& filePath, SPtr<const ImportOptions> importOptions, Vector<SubMesh>& subMeshes);

        /**
         * Parses an FBX scene. Find all meshes in the scene and returns mesh data object containing all vertices, indexes
         * and other mesh information. Also outputs a sub-mesh array that allows you locate specific sub-meshes within the
         * returned mesh data object. If requested animation and blend shape data is output as well.
         */
        void ParseScene(aiScene* scene, const AssimpImportOptions& options, AssimpImportScene& outputScene);

        /** Parses a mesh. Converts it from Assimp format into a mesh data object containing one or multiple sub-meshes. */
        void ParseMesh(aiMesh* mesh, AssimpImportNode* parentNode, const AssimpImportOptions& options, AssimpImportScene& outputScene);

        /** Converts the mesh data from the imported assimp scene into mesh data that can be used for initializing a mesh. */
        SPtr<RendererMeshData> GenerateMeshData(AssimpImportScene& scene, AssimpImportOptions& options, Vector<SubMesh>& subMeshes);

        /**	Creates an internal representation of an assimp node from an aiNode object. */
        AssimpImportNode* CreateImportNode(const AssimpImportOptions& options, AssimpImportScene& scene, aiNode* assimpNode, AssimpImportNode* parent);

        /** Convert an assimp matrix into engine matrix */
        Matrix4 ConvertToNativeType(const aiMatrix4x4& matrix);

        /** Convert an assimp vector4 into engine vector4 */
        Vector4 ConvertToNativeType(const aiColor4D& color);

        /** Convert an assimp vector3 into engine vector3 */
        Vector3 ConvertToNativeType(const aiVector3D& vector);

        /** Convert an assimp vector2 into engine vector2 */
        Vector2 ConvertToNativeType(const aiVector2D& vector);

    private:
        Vector<String> _extensions;
    };
}
