#pragma once

#include "TeFontImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "Importer/TeImporter.h"

namespace te
{
    /** Importer implementation that handles font import by using the FreeType library. */
    class FontImporter : public BaseImporter
    {
    public:
        FontImporter();
        virtual ~FontImporter() = default;

        /** @copydoc BaseImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc SpecificImporter::import */
        SPtr<Resource> Import(const String& filePath, SPtr<const ImportOptions> importOptions) override;

        /** @copydoc SpecificImporter::createImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;
    private:
        Vector<String> _extensions;

        const static int MAXIMUM_TEXTURE_SIZE = 2048;
    };
}
