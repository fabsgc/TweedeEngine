#include "TeOAImporter.h"
#include "Utility/TeFileStream.h"
#include "Audio/TeAudioClipImportOptions.h"

namespace te
{
    OAImporter::OAImporter()
        : BaseImporter()
    {
        _extensions.push_back(u8"wav");
        _extensions.push_back(u8"ogg");
        _extensions.push_back(u8"flac");
    }

    bool OAImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), tolower);
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> OAImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<AudioClipImportOptions>();
    }

    SPtr<Resource> OAImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        return SPtr<Resource>();
    }
}
