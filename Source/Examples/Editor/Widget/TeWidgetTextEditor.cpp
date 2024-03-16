#include "TeWidgetTextEditor.h"

#include "../TeEditor.h"
#include "../ImGuiExt/TeImGuiTextEditor.h"
#include "Utility/TeDataStream.h"
#include "Scripting/TeScript.h"

namespace te
{
    WidgetTextEditor::WidgetTextEditor()
        : Widget(WidgetType::TextEditor)
        , _selections(gEditor().GetSelectionData())
        , _editor(nullptr)
        , _needsSave(false)
    {
        _title = TEXT_EDITOR_TITLE;
    }

    WidgetTextEditor::~WidgetTextEditor()
    { }

    void WidgetTextEditor::Initialize()
    {
        _editor = te_shared_ptr_new<ImGuiTextEditor>();

        OnBuild = Event<void()>();
        OnSave = Event<void()>();
    }

    void WidgetTextEditor::Update()
    {
        ShowToolbar();
        ShowEditor();

        if (_editor->IsTextChanged())
        {
            _editorContent = _editor->GetText();
            _needsSave = true;
        }
    }

    void WidgetTextEditor::UpdateBackground()
    { }

    void WidgetTextEditor::PutFocus()
    {
        Widget::PutFocus();
        UpdateEditorContent(); // If current script name different from previous, we need to load its content
    }

    void WidgetTextEditor::UpdateEditorContent()
    {
        if (_script && !_script->GetPath().empty())
        {
            FileStream file(_script->GetPath());
            if (!file.Fail())
            {
                _editorContent = file.GetAsString();
                _editor->SetText(_editorContent);
                _needsSave = false;
                file.Close();
            }
        }
        else
        {
            _editor->SetText("");
            _needsSave = false;
        }
    }

    void WidgetTextEditor::SetScript(Script* script)
    {
        _script = script;
        UpdateEditorContent();
    }

    const Script* WidgetTextEditor::GetScript() const
    {
        return _script;
    }

    void WidgetTextEditor::ShowEditor()
    {
        ImGui::BeginChild("TextEditor", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() - 90.0f), false);
        _editor->Render("Title", ImVec2(0.0f, ImGui::GetContentRegionMax().y)); // shrink y to bring the compile button into view
        ImGui::EndChild();
    }

    void WidgetTextEditor::ShowToolbar()
    {
        auto ShowButton = [this](const char* title, bool enabled, const std::function<void()>& action)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (enabled) ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);
            if (ImGui::Button(title, ImVec2(32.0f, 26.0f)))
            {
                if(enabled)
                    action();
            }
            ImGui::PopStyleColor();
        };

        ImGui::BeginChild("TextEditorActionList", ImVec2(ImGui::GetContentRegionAvail().x, 42.0f), true);

        // Build current script
        ShowButton(ICON_FA_COGS, _script != nullptr, [this]() {
            OnBuild();
        });

        // Save current script
        ShowButton(ICON_FA_SAVE, _script != nullptr, [this]() {
            OnSave();
        });

        ImGui::EndChild();
    }
}
