#include "Importer/TeBaseImporter.h"

namespace te
{
    Vector<SubResourceRaw> BaseImporter::ImportAll(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        SPtr<Resource> resource = Import(filePath, importOptions);
        if (resource == nullptr)
            return Vector<SubResourceRaw>();

        return { { u8"primary", resource } };
    }

    SPtr<ImportOptions> BaseImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<ImportOptions>();
    }

    SPtr<const ImportOptions> BaseImporter::GetDefaultImportOptions() const
    {
        if (_defaultImportOptions == nullptr)
            _defaultImportOptions = CreateImportOptions();

        return _defaultImportOptions;
    }
}
