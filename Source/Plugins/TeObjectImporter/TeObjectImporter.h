#pragma once

#include "TeObjectImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "Utility/TeColor.h"
#include "Renderer/TeRendererMeshData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace te
{
    struct AssimpImportData
    {
        Vector<UINT32> Indices;
        Vector<Vector3> Positions;
        Vector<Vector3> Normals;
        Vector<Vector3> Tangents;
        Vector<Vector3> Bitangents;
        Vector<RGBA> Colors;
        Vector<Vector2> Textures;

        SPtr<MeshData> meshData;
        Vector<SubMesh> subMeshes;
    };

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
        SPtr<RendererMeshData> ImportMeshData(const aiScene* scene, SPtr<const ImportOptions> importOptions, Vector<SubMesh>& subMeshes);

    private:
        Vector<String> _extensions;
    };
}
