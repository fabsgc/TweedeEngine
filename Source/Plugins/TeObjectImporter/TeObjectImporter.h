#pragma once

#include "TeObjectImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"

namespace te
{
    class ObjectImporter : public BaseImporter
    {
    public:
        ObjectImporter();
        virtual ~ObjectImporter();

        /** @copydoc BasicImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc BasicImporter::Import */
        SPtr<Resource> Import(const String& filePath, const SPtr<const ImportOptions> importOptions) override;

        /** @copydoc BasicImporter::CreateImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;

    private:
        Vector<String> _extensions;
    };
}