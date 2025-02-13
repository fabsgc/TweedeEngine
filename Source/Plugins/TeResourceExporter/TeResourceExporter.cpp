#include "TeResourceExporter.h"
#include "Serialization/TeBinaryWriter.h"
#include "Exporter/TeResourceExportOptions.h"

namespace te
{ 
    ResourceExporter::ResourceExporter()
        : BaseExporter()
    {
        _extensions.push_back("resource");
    }

    bool ResourceExporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    bool ResourceExporter::Export(void* object, const std::filesystem::path& filePath, const ExportOptions& exportOptions, bool force)
    {
        const ResourceExportOptions& resourceExportOptions = static_cast<const ResourceExportOptions&>(exportOptions);

        Resource* resource = static_cast<Resource*>(object);

        BinaryWriter* resourceSerializer = te_new<BinaryWriter>(filePath);
        resource->SetPath(filePath);
        resource->Serialize(resourceSerializer);

        te_delete(resourceSerializer);

        return true;
    }
}
