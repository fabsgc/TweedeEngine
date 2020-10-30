#pragma once

#include "TeCorePrerequisites.h"
#include "TeImGuiExt.h"

namespace te
{
    class ImGuiFileDialog
    {
    public:
        ImGuiFileDialog();
        ~ImGuiFileDialog();

        bool Open(UINT32 type);
    };
}
