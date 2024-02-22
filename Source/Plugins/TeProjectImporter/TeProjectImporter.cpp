#include "TeProjectImporter.h"
#include "Importer/TeProjectImportOptions.h"

namespace te
{ 
    ProjectImporter::ProjectImporter()
        : BaseImporter()
    {
        _extensions.push_back(u8"scene");
    }

    bool ProjectImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> ProjectImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<ProjectImportOptions>();
    }

    SPtr<Resource> ProjectImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        const ProjectImportOptions* projectImportOptions = static_cast<const ProjectImportOptions*>(importOptions.get());

        return nullptr;
    }
}
