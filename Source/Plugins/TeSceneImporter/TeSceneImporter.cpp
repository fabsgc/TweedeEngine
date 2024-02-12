#include "TeSceneImporter.h"
#include "Importer/TeSceneImportOptions.h"

namespace te
{ 
    SceneImporter::SceneImporter()
        : BaseImporter()
    {
        _extensions.push_back(u8"scene");
    }

    bool SceneImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> SceneImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<SceneImportOptions>();
    }

    SPtr<Resource> SceneImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        const SceneImportOptions* sceneImportOptions = static_cast<const SceneImportOptions*>(importOptions.get());

        return nullptr;
    }
}
