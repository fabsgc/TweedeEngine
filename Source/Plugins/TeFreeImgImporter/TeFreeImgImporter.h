#pragma once

#include "TeFreeImgImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"

namespace te
{
    class FreeImgImporter : public BaseImporter
    {
    public:
        FreeImgImporter();
        virtual ~FreeImgImporter();

        /** @copydoc BasicImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc BasicImporter::Import */
        SPtr<Resource> Import(const String& filePath, SPtr<const ImportOptions> importOptions) override;

        /** @copydoc BasicImporter::CreateImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;

    private:
        Vector<String> _extensions;
    };
}