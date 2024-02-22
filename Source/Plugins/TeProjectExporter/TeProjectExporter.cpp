#include "TeProjectExporter.h"
#include "Exporter/TeProjectExportOptions.h"

namespace te
{ 
    ProjectExporter::ProjectExporter()
        : BaseExporter()
    {
        _extensions.push_back(u8"scene");
    }

    bool ProjectExporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ExportOptions> ProjectExporter::CreateExportOptions() const
    {
        return te_shared_ptr_new<ProjectExportOptions>();
    }

    bool ProjectExporter::Export(void* object, const String& filePath, SPtr<const ExportOptions> exportOptions, bool force)
    {
        const ProjectExportOptions* projectExportOptions = static_cast<const ProjectExportOptions*>(exportOptions.get());

        return true;
    }
}
