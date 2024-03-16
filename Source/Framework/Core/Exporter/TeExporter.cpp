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

    bool Exporter::Export(void* object, const String& inputFilePath, SPtr<const ExportOptions> exportOptions)
    {
        BaseExporter* importer = PrepareForExport(inputFilePath, exportOptions);
        if (!importer)
            return false;

        return importer->Export(object, inputFilePath, exportOptions);
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

    BaseExporter* Exporter::GetExporterForFile(const String& inputFilePath) const
    {
        String ext = Util::GetFileExtension(inputFilePath);

        if (ext.empty())
            return nullptr;

        ext = ext.substr(1, ext.size() - 1); // Remove the .
        if (!SupportsFileType(ext))
        {
            TE_ASSERT_ERROR(false, "There is no importer for the provided file" + inputFilePath);
            return nullptr;
        }

        for (const auto& assetExporter : _assetExporters)
        {
            if (assetExporter && assetExporter->IsExtensionSupported(ext))
                return assetExporter;
        }

        return nullptr;
    }

    BaseExporter* Exporter::PrepareForExport(const String& filePath, SPtr<const ExportOptions>& exportOptions) const
    {
        BaseExporter* exporter = GetExporterForFile(filePath);
        if (exporter == nullptr)
            return nullptr;

        if (exportOptions == nullptr)
        {
            exportOptions = exporter->GetDefaultExportOptions();
        }
        else
        {
            SPtr<const ExportOptions> defaultExportOptions = exporter->GetDefaultExportOptions();
            if (exportOptions->GetCoreType() != defaultExportOptions->GetCoreType())
            {
                TE_ASSERT_ERROR(false, "Provided import options is not of valid type. Expected: " + 
                    ToString(defaultExportOptions->GetCoreType()) + ". Got: " + ToString(exportOptions->GetCoreType()));
            }
        }

        return exporter;
    }

    TE_CORE_EXPORT Exporter& gExporter()
    {
        return Exporter::Instance();
    }
}
