#include "TeWidgetMenuBar.h"

#include "../TeEditorResManager.h"
#include "../TeEditor.h"
#include "../TeEditorUtils.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Audio/TeAudioClipImportOptions.h"
#include "String/TeUnicode.h"
#include "Image/TeTexture.h"
#include "Mesh/TeMesh.h"
#include "Physics/TePhysicsMesh.h"

#include <regex>

namespace te
{
    const String WidgetMenuBar::NEW_BINDING = "New";
    const String WidgetMenuBar::OPEN_BINDING = "Open";
    const String WidgetMenuBar::SAVE_BINDING = "Save";
    const String WidgetMenuBar::SAVE_AS_BINDING = "Save As";
    const String WidgetMenuBar::QUIT_BINDING = "Quit";
    const String WidgetMenuBar::LOAD_RESOURCE_BINDING = "Load Resource";

    WidgetMenuBar::WidgetMenuBar()
        : Widget(WidgetType::MenuBar)
        , _fileBrowser(gEditor().GetFileBrowser())
    { 
        _title = MENUBAR_TITLE;
        _isWindow = false;
    }

    WidgetMenuBar::~WidgetMenuBar()
    { }

    void WidgetMenuBar::Initialize()
    { 
        _newBtn = VirtualButton(NEW_BINDING);
        _openBtn = VirtualButton(OPEN_BINDING);
        _saveBtn = VirtualButton(SAVE_BINDING);
        _saveAsBtn = VirtualButton(SAVE_AS_BINDING);
        _quitBtn = VirtualButton(QUIT_BINDING);
        _loadResource = VirtualButton(LOAD_RESOURCE_BINDING);
    }

    void WidgetMenuBar::Update()
    {
        if (gVirtualInput().IsButtonDown(_newBtn))
            gEditor().GetSettings().State = Editor::EditorState::Modified;

        if (gVirtualInput().IsButtonDown(_openBtn))
            _settings.Open = true;

        if (gVirtualInput().IsButtonDown(_saveBtn))
            Save();
            
        if (gVirtualInput().IsButtonDown(_saveAsBtn))
            _settings.Save = true;

        if (gVirtualInput().IsButtonDown(_quitBtn))
            Quit();

        if (gVirtualInput().IsButtonDown(_loadResource))
            _settings.Load = true;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem(ICON_FA_FILE_ALT "   " ICON_FA_GRIP_LINES_VERTICAL "  New", "Ctrl+N"))
                { }

                ImGui::MenuItem(ICON_FA_FOLDER_OPEN " " ICON_FA_GRIP_LINES_VERTICAL "  Open", "Ctrl+O", &_settings.Open);

                if (ImGui::MenuItem(ICON_FA_SAVE "  " ICON_FA_GRIP_LINES_VERTICAL "  Save", "Ctrl+S"))
                    Save();

                ImGui::MenuItem(ICON_FA_SAVE "  " ICON_FA_GRIP_LINES_VERTICAL "  Save As ..", "Ctrl+Shift+S", _settings.Save);

                if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT "  " ICON_FA_GRIP_LINES_VERTICAL "  Quit", "Ctrl+Q"))
                    gCoreApplication().OnStopRequested();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Project"))
            {
                ImGui::MenuItem(ICON_FA_FILE_DOWNLOAD "  " ICON_FA_GRIP_LINES_VERTICAL "  Load resource", "Ctrl+R", &_settings.Load);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem(ICON_FA_BONG "  " ICON_FA_GRIP_LINES_VERTICAL "  Project"))
                {
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Project));
                    if(widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_TOOLS "  " ICON_FA_GRIP_LINES_VERTICAL "  Properties"))
                {
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Properties));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_IMAGE "  " ICON_FA_GRIP_LINES_VERTICAL "  Viewport"))
                {
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Viewport));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_TERMINAL " " ICON_FA_GRIP_LINES_VERTICAL "  Console"))
                { 
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Console));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_CHART_LINE "  " ICON_FA_GRIP_LINES_VERTICAL "  Profiler"))
                {
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Profiler));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_COGS "  " ICON_FA_GRIP_LINES_VERTICAL "  Settings"))
                {
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Settings));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_COG "  " ICON_FA_GRIP_LINES_VERTICAL "  Render options"))
                { 
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::RenderOptions));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_SUITCASE "  " ICON_FA_GRIP_LINES_VERTICAL "  Resources"))
                { 
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Resources));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_SCROLL " " ICON_FA_GRIP_LINES_VERTICAL "  Script"))
                { 
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Script));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem(ICON_FA_INFO_CIRCLE "  " ICON_FA_GRIP_LINES_VERTICAL "  About", nullptr, &_settings.AboutWindow);
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
        if (!_settings.AboutWindow)
            return;

        ImGui::SetNextWindowFocus();
        ImGui::Begin("About", &_settings.AboutWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

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
        if (_settings.Open)
            ImGui::OpenPopup("Open Project");

        if (_fileBrowser.ShowFileDialog("Open Project", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(800, 450), true, ".scene"))
        {
            gEditor().GetSettings().FilePath = _fileBrowser.Data.SelectedPath;
            gEditor().Open();
            _settings.Open = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _settings.Open = false;
        }
    }

    void WidgetMenuBar::ShowSave()
    {
        if (_settings.Save)
            ImGui::OpenPopup("Save Project");

        if (_fileBrowser.ShowFileDialog("Save Project", ImGuiFileBrowser::DialogMode::SAVE, ImVec2(800, 450), false, ".scene"))
        {
            if (!std::regex_match(_fileBrowser.Data.SelectedPath, std::regex("^(.*)\\.(scene)$")))
                gEditor().GetSettings().FilePath = _fileBrowser.Data.SelectedPath + ".scene";
            else
                gEditor().GetSettings().FilePath = _fileBrowser.Data.SelectedPath;
            
            gEditor().Save();
            _settings.Save = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _settings.Save = false;
        }
    }

    void WidgetMenuBar::ShowLoad()
    {
        if (_settings.Load)
            ImGui::OpenPopup("Load Resource");

        if (_fileBrowser.ShowFileDialog("Load Resource", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(800, 450), true, ".png,.jpeg,.jpg,.obj,.dae,.fbx,.stl,.gltf,.scene,.ogg,.wav,.flac"))
        {
            if (_fileBrowser.Data.Ext == ".jpeg" || _fileBrowser.Data.Ext == ".jpg" || _fileBrowser.Data.Ext == ".png")
            {
                auto textureImportOptions = TextureImportOptions::Create();
                if (_fileBrowser.Data.TexParam.TexType == TextureType::TEX_TYPE_CUBE_MAP)
                {
                    textureImportOptions->CpuCached = false;
                    textureImportOptions->CubemapType = CubemapSourceType::Faces;
                    textureImportOptions->IsCubemap = true;
                    textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
                }
                else
                {
                    textureImportOptions->CpuCached = false;
                    textureImportOptions->GenerateMips = _fileBrowser.Data.TexParam.GenerateMips;
                    textureImportOptions->MaxMip = _fileBrowser.Data.TexParam.MaxMips;
                    textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
                }

                HTexture texture = EditorResManager::Instance().Load<Texture>(_fileBrowser.Data.SelectedPath, textureImportOptions, true);
                if (texture.IsLoaded())
                {
                    texture->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                    EditorResManager::Instance().Add<Texture>(texture);
                }
            }
            else if (_fileBrowser.Data.Ext == ".obj" || _fileBrowser.Data.Ext == ".dae" || _fileBrowser.Data.Ext == ".fbx" || 
                     _fileBrowser.Data.Ext == ".stl" || _fileBrowser.Data.Ext == ".gltf")
            {
                auto meshImportOptions = MeshImportOptions::Create();
                meshImportOptions->ImportNormals = _fileBrowser.Data.MeshParam.ImportNormals;
                meshImportOptions->ImportTangents = _fileBrowser.Data.MeshParam.ImportTangents;
                meshImportOptions->ImportSkin = _fileBrowser.Data.MeshParam.ImportSkin;
                meshImportOptions->ImportBlendShapes = _fileBrowser.Data.MeshParam.ImportBlendShapes;
                meshImportOptions->ImportAnimations = _fileBrowser.Data.MeshParam.ImportAnimations;
                meshImportOptions->ReduceKeyFrames = _fileBrowser.Data.MeshParam.ReduceKeyFrames;
                meshImportOptions->ImportMaterials = _fileBrowser.Data.MeshParam.ImportMaterials;
                meshImportOptions->ImportVertexColors = _fileBrowser.Data.MeshParam.ImportVertexColors;
                meshImportOptions->ForceGenNormals = _fileBrowser.Data.MeshParam.ForceGenNormals;
                meshImportOptions->GenSmoothNormals = _fileBrowser.Data.MeshParam.GenSmoothNormals;
                meshImportOptions->ScaleSystemUnit = _fileBrowser.Data.MeshParam.ScaleSystemUnit;
                meshImportOptions->ScaleFactor = _fileBrowser.Data.MeshParam.ScaleFactor;
                meshImportOptions->CollisionShape = _fileBrowser.Data.MeshParam.CollisionShape;
                meshImportOptions->CpuCached = false;

                SPtr<MultiResource> resources = EditorResManager::Instance().LoadAll(_fileBrowser.Data.SelectedPath, meshImportOptions, true);
                if (!resources->Empty())
                {
                    for (auto& subRes : resources->Entries)
                    {
                        if (subRes.Name == "primary")
                        {
                            HMesh mesh = static_resource_cast<Mesh>(subRes.Res);
                            if (mesh.IsLoaded())
                            {
                                mesh->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                                EditorResManager::Instance().Add<Mesh>(mesh);

                                if (_fileBrowser.Data.MeshParam.ImportMaterials)
                                    EditorUtils::ImportMeshMaterials(mesh);
                            }
                        }
                        else if (subRes.Name == "collision")
                        {
                            HPhysicsMesh physicsMesh = static_resource_cast<PhysicsMesh>(subRes.Res);
                            if (physicsMesh.IsLoaded())
                            {
                                EditorResManager::Instance().Add<PhysicsMesh>(physicsMesh);
                            }
                        }
                        else
                        {
                            subRes.Res->SetPath(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                        }
                    }
                }
            }
            else if (_fileBrowser.Data.Ext == ".ogg" || _fileBrowser.Data.Ext == ".wav" || _fileBrowser.Data.Ext == ".flac")
            {
                auto audioImportOptions = AudioClipImportOptions::Create();
                audioImportOptions->Is3D = _fileBrowser.Data.AudioParam.Is3D;

                HAudioClip audio = EditorResManager::Instance().Load<AudioClip>(_fileBrowser.Data.SelectedPath, audioImportOptions, true);
                if (audio.IsLoaded())
                {
                    audio->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                    EditorResManager::Instance().Add<AudioClip>(audio);
                }
            }
            else
            {
                // TODO scene loading
            }

            _settings.Load = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _settings.Load = false;
        }
    }

    void WidgetMenuBar::Quit()
    {
        if (gEditor().GetSettings().State == Editor::EditorState::Modified)
        {
            // TODO quit app
        }

        gCoreApplication().OnStopRequested();
    }

    void WidgetMenuBar::Save()
    {
        if (gEditor().GetSettings().State == Editor::EditorState::Modified)
        {
            if (gEditor().GetSettings().FilePath.empty())
                _settings.Save = true;
            else
                gEditor().Save();
        }
    }
}
