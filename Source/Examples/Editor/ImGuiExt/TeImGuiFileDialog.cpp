#include "TeImGuiFileDialog.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace te
{
    ImGuiFileDialog::ImGuiFileDialog()
        : _visible(false)
        , _mode(OpenMode::Open)
    { }

    ImGuiFileDialog::~ImGuiFileDialog()
    { }

    bool ImGuiFileDialog::Open(const String& attachedPopup)
    {
        if (!_visible)
            return false;

        bool returnCode = false;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 maxSize;
        ImVec2 minSize(400.0f, 250.0f);
        maxSize.x = io.DisplaySize.x;
        maxSize.y = io.DisplaySize.y;
        ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400.0f, 250.0f), ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal(attachedPopup.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            if (ImGui::Button("Close"))
            {
                _visible = false; //hide popup
                returnCode = true;
                _file = FileStream(); // reset file as nothing has been selected
                
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return returnCode;
    }

    void ImGuiFileDialog::SetFilters(Vector<String>& filters)
    {
        _filters = filters;
    }

    void ImGuiFileDialog::ResetFilters()
    {
        _filters.clear();
    }
}
