#pragma once

#include "TeFreeImgExporterPrerequisites.h"
#include "Exporter/TeBaseExporter.h"
#include "Image/TePixelData.h"
#include "FreeImage.h"

namespace te
{
    /** Exporter implementation that handles scene export. */
    class FreeImgExporter : public BaseExporter
    {
    public:
        FreeImgExporter();
        virtual ~FreeImgExporter();

        /** @copydoc BaseExporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc SpecificExporter::import */
        bool Export(void* object, const String& filePath, SPtr<const ExportOptions> importOptions, bool force = false) override;

        /** @copydoc SpecificExporter::createExportOptions */
        SPtr<ExportOptions> CreateExportOptions() const override;

    private:
        Vector<String> _extensions;
        UnorderedMap<String, int> _extensionToFID;
    };
}
