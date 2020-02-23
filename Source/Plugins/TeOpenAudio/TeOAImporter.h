#pragma once

#include "TeOAPrerequisites.h"
#include "Importer/TeBaseImporter.h"

namespace te
{
    /** Importer using for importing WAV/FLAC/OGGVORBIS audio files. */
    class OAImporter : public BaseImporter
    {
    public:
        OAImporter();
        virtual ~OAImporter() = default;

        /** @copydoc BaseImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc BasicImporter::Import */
        SPtr<Resource> Import(const String& filePath, const SPtr<const ImportOptions> importOptions) override;

        /** @copydoc BasicImporter::CreateImportOptions */
        SPtr<ImportOptions> CreateImportOptions() const override;

    private:
        Vector<String> _extensions;
    };
}
