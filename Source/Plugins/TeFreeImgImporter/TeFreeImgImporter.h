#pragma once

#include "TeFreeImgImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "Image/TePixelData.h"

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
        SPtr<Resource> Import(const String& filePath, const SPtr<const ImportOptions> importOptions) override;

        /** @copydoc BasicImporter::CreateImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;

    private:
        /**	Converts a magic number into an extension name. */
        String MagicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;

        /**	Imports an image from the provided data stream. */
        SPtr<PixelData> ImportRawImage(const String& filePath);

    private:
        Vector<String> _extensions;
        Map<String, int> _extensionToFID;
    };
}