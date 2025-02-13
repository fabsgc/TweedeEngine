#include "Exporter/TeExporter.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Exporter)

    Exporter::Exporter()
    { }

    Exporter::~Exporter()
    {
        for (const auto& assetExporter : _assetExporters)
        {
            te_safe_delete(assetExporter);
        }
    }

    bool Exporter::Export(void* object, const std::filesystem::path& filePath, const ExportOptions& exportOptions)
    {
        BaseExporter* importer = PrepareForExport(filePath);
        if (!importer)
            return false;

        return importer->Export(object, filePath, exportOptions);
    }

    bool Exporter::SupportsFileType(const String& extension) const
    {
        for (const auto& assetExporter : _assetExporters)
        {
            if (assetExporter && assetExporter->IsExtensionSupported(extension))
                return true;
        }

        return false;
    }

    void Exporter::RegisterAssetExporter(BaseExporter* exporter)
    {
        if (!exporter)
        {
            TE_ASSERT_ERROR((exporter == nullptr), "Trying to register a null asset exporter!");
            return;
        }

        _assetExporters.push_back(exporter);
    }

    BaseExporter* Exporter::GetExporterForFile(const std::filesystem::path& filePath) const
    {
        String ext = Util::GetFileExtension(filePath.generic_string());

        if (ext.empty())
            return nullptr;

        ext = ext.substr(1, ext.size() - 1); // Remove the .
        if (!SupportsFileType(ext))
        {
            TE_ASSERT_ERROR(false, "There is no exporter for the provided file : " + filePath.generic_string());
            return nullptr;
        }

        for (const auto& assetExporter : _assetExporters)
        {
            if (assetExporter && assetExporter->IsExtensionSupported(ext))
                return assetExporter;
        }

        return nullptr;
    }

    BaseExporter* Exporter::PrepareForExport(const std::filesystem::path& filePath) const
    {
        return GetExporterForFile(filePath);
    }

    TE_CORE_EXPORT Exporter& gExporter()
    {
        return Exporter::Instance();
    }
}
