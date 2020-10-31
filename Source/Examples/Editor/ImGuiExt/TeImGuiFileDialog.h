#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeFileStream.h"

namespace te
{
    class ImGuiFileDialog
    {
    public:
        enum class OpenMode
        {
            Open, Save
        };

    public:
        ImGuiFileDialog();
        ~ImGuiFileDialog();

        /** Activate file dialog visibility */
        void SetVisible(bool visible) { _visible = visible; }

        /** Change display mode */
        void SetMode(OpenMode mode = OpenMode::Open) { _mode = mode; }

        /** Get display mode */
        OpenMode GetMode() { return _mode; }

        /** Retrives data about selected file */
        FileStream& GetFile() { return _file; }

        /** Returns true if a file has been selected */
        bool Open(const String& attachedPopup = "Popup");

        /** You can filter extension you want to see in file dialog */
        void SetFilters(Vector<String>& filters);

        /** You can reset filters applied on files display */
        void ResetFilters();

    protected:
        bool _visible;
        OpenMode _mode;
        FileStream _file;
        Vector<String> _filters;
    };
}
