#include "TeSceneExporter.h"
#include "Exporter/TeSceneExportOptions.h"

namespace te
{ 
    SceneExporter::SceneExporter()
        : BaseExporter()
    {
        _extensions.push_back(u8"scene");
    }

    bool SceneExporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ExportOptions> SceneExporter::CreateExportOptions() const
    {
        return te_shared_ptr_new<SceneExportOptions>();
    }

    bool SceneExporter::Export(void* object, const String& filePath, SPtr<const ExportOptions> exportOptions, bool force)
    {
        const SceneExportOptions* sceneExportOptions = static_cast<const SceneExportOptions*>(exportOptions.get());

        return true;
    }
}
