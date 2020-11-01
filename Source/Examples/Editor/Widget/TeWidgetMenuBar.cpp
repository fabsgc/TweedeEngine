#include "TeWidgetMenuBar.h"

#include "../TeEditor.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "../TeEditorResManager.h"
#include "Image/TeTexture.h"
#include "Mesh/TeMesh.h"

namespace te
{
    WidgetMenuBar::WidgetMenuBar()
        : Widget(WidgetType::MenuBar)
        , _fileBrowser(Editor::Instance().GetFileBrowser())
        , _open(false)
        , _save(false)
        , _load(false)
    { 
        _title = MENUBAR_TITLE;
        _isWindow = false;
    }

    WidgetMenuBar::~WidgetMenuBar()
    { }

    void WidgetMenuBar::Initialize()
    { 
        _newBtn = VirtualButton("New");
        _openBtn = VirtualButton("Open");
        _saveBtn = VirtualButton("Save");
        _saveAsBtn = VirtualButton("Save As");
        _quitBtn = VirtualButton("Quit");
        _loadResource = VirtualButton("LoadResource");
    }

    void WidgetMenuBar::Update()
    {
        if (gVirtualInput().IsButtonDown(_newBtn))
        { }

        if (gVirtualInput().IsButtonDown(_openBtn))
            _open = true;

        if (gVirtualInput().IsButtonDown(_saveBtn))
            _save = true; // TODO open file browser if we save for the first time

        if (gVirtualInput().IsButtonDown(_saveAsBtn))
            _save = true;

        if(gVirtualInput().IsButtonDown(_quitBtn))
            gCoreApplication().OnStopRequested();

        if (gVirtualInput().IsButtonDown(_loadResource))
            _load = true;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem(ICON_FA_FILE_ALT "   " ICON_FA_GRIP_LINES_VERTICAL "  New", "Ctrl+N"))
                { }

                if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " " ICON_FA_GRIP_LINES_VERTICAL "  Open", "Ctrl+O"))
                    _open = true;

                if (ImGui::MenuItem(ICON_FA_SAVE "  " ICON_FA_GRIP_LINES_VERTICAL "  Save", "Ctrl+S"))
                    _save = true;

                if (ImGui::MenuItem(ICON_FA_SAVE "  " ICON_FA_GRIP_LINES_VERTICAL "  Save As ..", "Ctrl+Shift+S"))
                    _save = true;

                if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT "  " ICON_FA_GRIP_LINES_VERTICAL "  Quit", "Ctrl+Q"))
                    gCoreApplication().OnStopRequested();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Project"))
            {
                if (ImGui::MenuItem(ICON_FA_FILE_DOWNLOAD "  " ICON_FA_GRIP_LINES_VERTICAL "  Load resource", "Ctrl+R"))
                    _load = true;

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem(ICON_FA_BONG "  " ICON_FA_GRIP_LINES_VERTICAL "  Project"))
                {
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Project));
                    if(widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_TOOLS "  " ICON_FA_GRIP_LINES_VERTICAL "  Properties"))
                {
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Properties));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_IMAGE "  " ICON_FA_GRIP_LINES_VERTICAL "  Viewport"))
                {
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Viewport));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_TERMINAL " " ICON_FA_GRIP_LINES_VERTICAL "  Console"))
                { 
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Console));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_COG "  " ICON_FA_GRIP_LINES_VERTICAL "  Render options"))
                { 
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::RenderOptions));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_SUITCASE "  " ICON_FA_GRIP_LINES_VERTICAL "  Resources"))
                { 
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Resources));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_SCROLL " " ICON_FA_GRIP_LINES_VERTICAL "  Script"))
                { 
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Script));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_GAMEPAD " " ICON_FA_GRIP_LINES_VERTICAL "  Game"))
                {
                    Widget* widget = static_cast<Widget*>(Editor::Instance().GetWidget(WidgetType::Game));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem(ICON_FA_INFO_CIRCLE "  " ICON_FA_GRIP_LINES_VERTICAL "  About", nullptr, &_settings.ShowAboutWindow);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ShowAboutWindow();
        ShowOpen();
        ShowSave();
        ShowLoad();
    }

    void WidgetMenuBar::UpdateBackground()
    { }

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

    void WidgetMenuBar::ShowOpen()
    {
        if (_open)
            ImGui::OpenPopup("Open Project");

        if (_fileBrowser.ShowFileDialog("Open Project", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(800, 450), ".scene"))
        {
            _open = false;
        }
        else
        {
            if (_fileBrowser.IsCancelled)
                _open = false;
        }
    }

    void WidgetMenuBar::ShowSave()
    {
        if (_save)
            ImGui::OpenPopup("Save Project");

        if (_fileBrowser.ShowFileDialog("Save Project", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(800, 450), ".scene"))
        {
            _save = false;
        }
        else
        {
            if (_fileBrowser.IsCancelled)
                _save = false;
        }
    }

    void WidgetMenuBar::ShowLoad()
    {
        if (_load)
            ImGui::OpenPopup("Load Resource");

        if (_fileBrowser.ShowFileDialog("Load Resource", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(800, 450), ".jpeg,.jpg,.png,.obj,.dae,.fbx,.scene"))
        {
            if (_fileBrowser.ext == ".jpeg" || _fileBrowser.ext == ".jpg" || _fileBrowser.ext == ".png")
            {
                auto textureImportOptions = TextureImportOptions::Create();
                textureImportOptions->CpuCached = false;
                textureImportOptions->GenerateMips = true;

                HTexture texture = EditorResManager::Instance().Load<Texture>(_fileBrowser.SelectedPath, textureImportOptions);

                if (texture.GetHandleData())
                {
                    texture->SetName(_fileBrowser.SelectedFileName);
                    EditorResManager::Instance().Add<Texture>(texture);
                }
            }
            else if (_fileBrowser.ext == ".obj" || _fileBrowser.ext == ".dae" || _fileBrowser.ext == ".fbx")
            {
                auto meshImportOptions = MeshImportOptions::Create();
                meshImportOptions->ImportNormals = true;
                meshImportOptions->ImportTangents = true;
                meshImportOptions->ImportSkin = true;
                meshImportOptions->ImportAnimation = true;
                meshImportOptions->CpuCached = false;

                HMesh mesh = EditorResManager::Instance().Load<Mesh>(_fileBrowser.SelectedPath, meshImportOptions);

                if (mesh.GetHandleData())
                {
                    mesh->SetName(_fileBrowser.SelectedFileName);
                    EditorResManager::Instance().Add<Mesh>(mesh);
                }
            }
            else
            {
                // TODO scene
            }

            _load = false;
        }
        else
        {
            if (_fileBrowser.IsCancelled)
                _load = false;
        }
    }
}
