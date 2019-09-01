#include "TeObjectImporter.h"
#include "Importer/TeMeshImportOptions.h"

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
        return SPtr<Resource>();
    }
}