#include "Importer/TeBaseImporter.h"

namespace te
{
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