#include "TeWidgetMenuBar.h"

#include "../TeEditor.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace te
{
    WidgetMenuBar::WidgetMenuBar()
        : Widget(WidgetType::MenuBar)
    { 
        _title = MENUBAR_TITLE;
        _isWindow = false;
    }

    WidgetMenuBar::~WidgetMenuBar()
    { }

    void WidgetMenuBar::Initialize()
    { 
        _newBtn = VirtualButton("New");
        _quitBtn = VirtualButton("Quit");
    }

    void WidgetMenuBar::Update()
    {
        if (gVirtualInput().IsButtonDown(_newBtn))
        { }

        if(gVirtualInput().IsButtonHeld(_quitBtn))
            gCoreApplication().OnStopRequested();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                { }

                if (ImGui::MenuItem("Quit", "Ctrl+Q"))
                    gCoreApplication().OnStopRequested();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Project"))
            {
                if (ImGui::MenuItem("Load resource"))
                { }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Project"))
                {
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Project));
                    if(widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem("Properties"))
                {
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Properties));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem("Viewport"))
                {
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Viewport));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem("Console"))
                { 
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Console));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem("Render options"))
                { 
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::RenderOptions));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem("Resources"))
                { 
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Resources));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem("About", nullptr, &_settings.ShowAboutWindow);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ShowAboutWindow();
    }

    void WidgetMenuBar::ShowAboutWindow()
    {
        if (!_settings.ShowAboutWindow)
            return;

        ImGui::SetNextWindowFocus();
        ImGui::Begin("About", &_settings.ShowAboutWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

        ImGui::Text("Tweede Framework Redux");
        ImGui::Text("Author: Fabien Beaujean");
        ImGui::SameLine(ImGui::GetWindowContentRegionWidth());
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 55);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);

        ImGui::Separator();

        ImGui::BeginChildFrame(ImGui::GetID("about_license"), ImVec2(575, ImGui::GetTextLineHeightWithSpacing() * 15.5f), ImGuiWindowFlags_NoMove);
        ImGui::Text("MIT License");
        ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
        ImGui::Text("of this software and associated documentation files(the \"Software\"), to deal");
        ImGui::Text("in the Software without restriction, including without limitation the rights");
        ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and / or sell");
        ImGui::Text("copies of the Software, and to permit persons to whom the Software is furnished");
        ImGui::Text("to do so, subject to the following conditions :");
        ImGui::Text("The above copyright notice and this permission notice shall be included in");
        ImGui::Text("all copies or substantial portions of the Software.");
        ImGui::Text("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
        ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS");
        ImGui::Text("FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR");
        ImGui::Text("COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER");
        ImGui::Text("IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN");
        ImGui::Text("CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
        ImGui::EndChildFrame();

        ImGui::Separator();

        ImGui::End();
    }
}
