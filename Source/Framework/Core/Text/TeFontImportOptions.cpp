#include "Text/TeFontImportOptions.h"

namespace te
{
    FontImportOptions::FontImportOptions()
        : ImportOptions(TID_FontImportOptions)
    { }

    SPtr<FontImportOptions> FontImportOptions::Create()
    {
        return te_shared_ptr_new<FontImportOptions>();
    }
}
