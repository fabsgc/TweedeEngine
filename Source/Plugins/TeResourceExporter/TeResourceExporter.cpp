#include "TeResourceExporter.h"
#include "Exporter/TeResourceExportOptions.h"

namespace te
{ 
    ResourceExporter::ResourceExporter()
        : BaseExporter()
    {
        _extensions.push_back(u8"resource");
    }

    bool ResourceExporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ExportOptions> ResourceExporter::CreateExportOptions() const
    {
        return te_shared_ptr_new<ResourceExportOptions>();
    }

    bool ResourceExporter::Export(void* object, const String& filePath, SPtr<const ExportOptions> exportOptions, bool force)
    {
        const ResourceExportOptions* resourceExportOptions = static_cast<const ResourceExportOptions*>(exportOptions.get());

        return true;
    }
}
