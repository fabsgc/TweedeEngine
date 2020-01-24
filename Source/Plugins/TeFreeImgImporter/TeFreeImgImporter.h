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
        /** Converts a magic number into an extension name. */
        String MagicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;

        /** Imports an image from the provided data stream. */
        SPtr<PixelData> ImportRawImage(const String& filePath);

        /**
         * Generates six cubemap faces from the provided source texture. *
         *
         * @param[in]	source		Source texture containing the pixels to generate the cubemap from.
         * @param[in]	sourceType	Type of the source texture, determines how is the data interpreted.
         * @param[out]	output		Will contain the six cubemap faces, if the method returns true. The faces will be in the
         *							same order as presented in the CubemapFace enum.
         * @return					True if the cubemap faces were successfully generated, false otherwise.
         */
        bool GenerateCubemap(const SPtr<PixelData>& source, CubemapSourceType sourceType,
            std::array<SPtr<PixelData>, 6>& output);

    private:
        Vector<String> _extensions;
        Map<String, int> _extensionToFID;
    };
}