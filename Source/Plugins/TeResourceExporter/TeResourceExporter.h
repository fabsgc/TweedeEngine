#pragma once

#include "TeResourceExporterPrerequisites.h"
#include "Exporter/TeBaseExporter.h"
#include "Exporter/TeExporter.h"

namespace te
{
    /** Exporter implementation that handles scene export. */
    class ResourceExporter : public BaseExporter
    {
    public:
        ResourceExporter();
        virtual ~ResourceExporter() = default;

        /** @copydoc BaseExporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc SpecificExporter::import */
        bool Export(void* object, const String& filePath, SPtr<const ExportOptions> importOptions, bool force = false) override;

        /** @copydoc SpecificExporter::createExportOptions */
        SPtr<ExportOptions> CreateExportOptions() const override;
    private:
        Vector<String> _extensions;
    };
}
