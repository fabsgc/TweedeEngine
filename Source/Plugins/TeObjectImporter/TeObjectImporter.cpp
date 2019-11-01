#include "TeObjectImporter.h"
#include "Importer/TeMeshImportOptions.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace te
{
    ObjectImporter::ObjectImporter()
    {
        _extensions.push_back(u8"obj");
    }

    ObjectImporter::~ObjectImporter()
    {
    }

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

        return SPtr<Resource>();
    }
}
