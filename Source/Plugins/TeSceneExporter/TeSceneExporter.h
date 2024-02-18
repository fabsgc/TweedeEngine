#pragma once

#include "TeSceneExporterPrerequisites.h"
#include "Exporter/TeBaseExporter.h"
#include "Exporter/TeExporter.h"

namespace te
{
    /** Exporter implementation that handles scene export. */
    class SceneExporter : public BaseExporter
    {
    public:
        SceneExporter();
        virtual ~SceneExporter() = default;

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
