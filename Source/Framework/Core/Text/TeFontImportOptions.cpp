#include "Text/TeFontImportOptions.h"

namespace te
{
    SPtr<FontImportOptions> FontImportOptions::Create()
    {
        return te_shared_ptr_new<FontImportOptions>();
    }
}
