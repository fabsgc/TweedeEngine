#pragma once

#include "TeProjectImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "Importer/TeImporter.h"

namespace te
{
    /** Importer implementation that handles scene import. */
    class ProjectImporter : public BaseImporter
    {
    public:
        ProjectImporter();
        virtual ~ProjectImporter() = default;

        /** @copydoc BaseImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc SpecificImporter::import */
        SPtr<Resource> Import(const String& filePath, SPtr<const ImportOptions> importOptions) override;

        /** @copydoc SpecificImporter::createImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;
    private:
        Vector<String> _extensions;
    };
}
