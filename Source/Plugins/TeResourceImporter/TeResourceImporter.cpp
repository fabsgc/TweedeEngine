#include "TeResourceImporter.h"
#include "Importer/TeResourceImportOptions.h"

namespace te
{ 
    ResourceImporter::ResourceImporter()
        : BaseImporter()
    {
        _extensions.push_back(u8"resource");
    }

    bool ResourceImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> ResourceImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<ResourceImportOptions>();
    }

    SPtr<Resource> ResourceImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        const ResourceImportOptions* resourceImportOptions = static_cast<const ResourceImportOptions*>(importOptions.get());

        return nullptr;
    }
}
