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
        bool Export(void* object, const String& filePath, SPtr<const ExportOptions> exportOptions, bool force = false) override;

        /** @copydoc SpecificExporter::createExportOptions */
        SPtr<ExportOptions> CreateExportOptions() const override;

    private:
        /** Export a single PixelData to a speficied output file */
        bool Export(const PixelData& pixelData, UINT32 width, UINT32 height, bool isSRGB, const String& filePath, const TextureExportOptions* exportOptions, bool force = false);

    private:
        Vector<String> _extensions;
        UnorderedMap<String, int> _extensionToFID;
    };
}
