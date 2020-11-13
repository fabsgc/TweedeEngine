#include "Importer/TeImportOptions.h"

namespace te
{
    ImportOptions::ImportOptions()
        : Serializable(TID_ImportOptions)
    { }

    ImportOptions::ImportOptions(UINT32 type)
        : Serializable(type)
    { }
}
