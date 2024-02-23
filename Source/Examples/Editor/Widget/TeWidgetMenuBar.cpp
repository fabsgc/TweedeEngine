#include "TeWidgetMenuBar.h"

#include "../TeEditorResManager.h"
#include "../TeEditor.h"
#include "../TeEditorUtils.h"
#include "../ImGuiExt/TeImGuiFileBrowser.h"
#include "Mesh/TeMesh.h"
#include "Image/TeTexture.h"
#include "Image/TePixelUtil.h"
#include "String/TeUnicode.h"
#include "Physics/TePhysicsMesh.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Audio/TeAudioClipImportOptions.h"
#include "Project/TeProject.h"

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
            NewProject();

        if (gVirtualInput().IsButtonDown(_openBtn))
            OpenProject();

        if (gVirtualInput().IsButtonDown(_saveAsBtn))
            _settings.SaveProject = true;
        else if (gVirtualInput().IsButtonDown(_saveBtn))
            SaveProject();

        if (gVirtualInput().IsButtonDown(_quitBtn))
            Quit();

        if (gVirtualInput().IsButtonDown(_loadResource))
            _settings.LoadResource = true;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem(ICON_FA_FILE_ALT "   " ICON_FA_GRIP_LINES_VERTICAL "  New project", "Ctrl+N"))
                    NewProject();

                if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " " ICON_FA_GRIP_LINES_VERTICAL "  Open project", "Ctrl+O"))
                    OpenProject();

                if (ImGui::MenuItem(ICON_FA_SAVE "  " ICON_FA_GRIP_LINES_VERTICAL "  Save project", "Ctrl+S"))
                    SaveProject();

                if (ImGui::MenuItem(ICON_FA_SAVE "  " ICON_FA_GRIP_LINES_VERTICAL "  Save project As ..", "Ctrl+Shift+S"))
                    SaveProjectAs();

                if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT "  " ICON_FA_GRIP_LINES_VERTICAL "  Quit", "Ctrl+Q"))
                    Quit();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Project"))
            {
                ImGui::MenuItem(ICON_FA_FILE_DOWNLOAD "  " ICON_FA_GRIP_LINES_VERTICAL "  Load resource", "Ctrl+R", &_settings.LoadResource);
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

                if (ImGui::MenuItem(ICON_FA_PALETTE "  " ICON_FA_GRIP_LINES_VERTICAL "  Materials"))
                {
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Material));
                    if (widget)
                        widget->SetVisible(!widget->GetVisible());
                }

                if (ImGui::MenuItem(ICON_FA_PAINT_BRUSH "  " ICON_FA_GRIP_LINES_VERTICAL "  Shaders"))
                {
                    Widget* widget = static_cast<Widget*>(gEditor().GetWidget(WidgetType::Shaders));
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

        ShowSavePreviousModal();

        ShowAboutWindow();
        ShowSaveProject();
        ShowNewProject();
        ShowOpenProject();
        ShowLoadResource();
    }

    void WidgetMenuBar::UpdateBackground()
    { }

    void WidgetMenuBar::ShowAboutWindow()
    {
        if (!_settings.AboutWindow)
            return;

        ImGui::SetNextWindowFocus();
        ImGui::Begin("About", &_settings.AboutWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

        ImGui::Text("Tweede Engine");
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

    void WidgetMenuBar::ShowNewProject()
    {
        if (_settings.NewProject)
            ImGui::OpenPopup("New Project");

        if (_fileBrowser.ShowFileDialog("New Project", ImGuiFileBrowser::DialogMode::SELECT, ImVec2(900, 450), true, ""))
        {
            gEditor().NewProject(_fileBrowser.Data.SelectedPath + "project.json");
            _settings.NewProject = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _settings.NewProject = false;
        }
    }

    void WidgetMenuBar::ShowOpenProject()
    {
        if (_settings.OpenProject)
            ImGui::OpenPopup("Open Project");

        if (_fileBrowser.ShowFileDialog("Open Project", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, ".project"))
        {
            gEditor().OpenProject(_fileBrowser.Data.SelectedPath);
            _settings.OpenProject = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _settings.OpenProject = false;
        }
    }

    void WidgetMenuBar::ShowSaveProject()
    {
        if (_settings.SaveProject)
            ImGui::OpenPopup("Save Project");

        if (_fileBrowser.ShowFileDialog("Save Project", ImGuiFileBrowser::DialogMode::SAVE, ImVec2(900, 450), false, ".project"))
        {
            if (!std::regex_match(_fileBrowser.Data.SelectedPath, std::regex("^(.*)\\.(project)$")))
                gEditor().SaveProject(_fileBrowser.Data.SelectedPath + ".project");
            else
                gEditor().SaveProject(_fileBrowser.Data.SelectedPath);

            _settings.SaveProject = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _settings.SaveProject = false;
        }
    }

    void WidgetMenuBar::ShowLoadResource()
    {
        if (_settings.LoadResource)
            ImGui::OpenPopup("Load Resource");

        String extensions = Editor::TexturesExtensionsStr + "," + Editor::MeshesExtensionsStr + "," + Editor::SoundsExtensionsStr;
        if (_fileBrowser.ShowFileDialog("Load Resource", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, extensions))
        {
            if (std::find(Editor::TexturesExtensions.begin(), Editor::TexturesExtensions.end(), _fileBrowser.Data.SelectedFileExt) != Editor::TexturesExtensions.end())
            {
                auto textureImportOptions = TextureImportOptions::Create();
                textureImportOptions->CpuCached = _fileBrowser.Data.TexParam.CpuCached;
                textureImportOptions->GenerateMips = _fileBrowser.Data.TexParam.GenerateMips;
                textureImportOptions->GenerateMipsOnGpu = _fileBrowser.Data.TexParam.GenerateMipsOnGpu;
                textureImportOptions->MaxMip = _fileBrowser.Data.TexParam.MaxMips;
                textureImportOptions->MipsPreserveCoverage = _fileBrowser.Data.TexParam.MipsPreserveCoverage;
                textureImportOptions->Format = PixelUtil::BestFormatFromFile(_fileBrowser.Data.SelectedPath);
                textureImportOptions->SRGB = _fileBrowser.Data.TexParam.SRGB;
                textureImportOptions->IsNormalMap = _fileBrowser.Data.TexParam.IsNormalMap;

                if (_fileBrowser.Data.TexParam.TexType == TextureType::TEX_TYPE_CUBE_MAP)
                {
                    textureImportOptions->CubemapType = CubemapSourceType::Faces;
                    textureImportOptions->IsCubeMap = true;
                }

                HTexture texture = EditorResManager::Instance().Load<Texture>(_fileBrowser.Data.SelectedPath, textureImportOptions, true);
                if (texture.IsLoaded())
                {
                    texture->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                    EditorResManager::Instance().Add<Texture>(texture);
                }
            }
            else if (std::find(Editor::MeshesExtensions.begin(), Editor::MeshesExtensions.end(), _fileBrowser.Data.SelectedFileExt) != Editor::MeshesExtensions.end())
            {
                auto meshImportOptions = MeshImportOptions::Create();
                meshImportOptions->ImportNormals = _fileBrowser.Data.MeshParam.ImportNormals;
                meshImportOptions->ImportTangents = _fileBrowser.Data.MeshParam.ImportTangents;
                meshImportOptions->ImportUVCoords = _fileBrowser.Data.MeshParam.ImportUVCoords;
                meshImportOptions->ImportSkin = _fileBrowser.Data.MeshParam.ImportSkin;
                meshImportOptions->ImportBlendShapes = _fileBrowser.Data.MeshParam.ImportBlendShapes;
                meshImportOptions->ImportAnimations = _fileBrowser.Data.MeshParam.ImportAnimations;
                meshImportOptions->ReduceKeyFrames = _fileBrowser.Data.MeshParam.ReduceKeyFrames;
                meshImportOptions->ImportMaterials = _fileBrowser.Data.MeshParam.ImportMaterials;
                meshImportOptions->ImportTextures = _fileBrowser.Data.MeshParam.ImportTextures;
                meshImportOptions->ImportSRGBTextures = _fileBrowser.Data.MeshParam.ImportSRGBTextures;
                meshImportOptions->ImportVertexColors = _fileBrowser.Data.MeshParam.ImportVertexColors;
                meshImportOptions->ForceGenNormals = _fileBrowser.Data.MeshParam.ForceGenNormals;
                meshImportOptions->GenSmoothNormals = _fileBrowser.Data.MeshParam.GenSmoothNormals;
                meshImportOptions->ScaleSystemUnit = _fileBrowser.Data.MeshParam.ScaleSystemUnit;
                meshImportOptions->ScaleFactor = _fileBrowser.Data.MeshParam.ScaleFactor;
                meshImportOptions->ImportCollisionShape = _fileBrowser.Data.MeshParam.ImportCollisionShape;
                meshImportOptions->ImportZPrepassMesh = _fileBrowser.Data.MeshParam.ImportZPrepassMesh;
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

                                if (_fileBrowser.Data.MeshParam.ImportMaterials && _fileBrowser.Data.MeshParam.ImportTextures)
                                    EditorUtils::ImportMeshMaterials(mesh, meshImportOptions->ImportSRGBTextures);
                                else
                                    EditorUtils::ApplyDefaultMaterial(mesh);
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
            else if (std::find(Editor::SoundsExtensions.begin(), Editor::SoundsExtensions.end(), _fileBrowser.Data.SelectedFileExt) != Editor::SoundsExtensions.end())
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
                // TODO engine resource loading (Native HResource)
            }

            _settings.LoadResource = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _settings.LoadResource = false;
        }
    }

    void WidgetMenuBar::ShowSavePreviousModal()
    {
        if (_settings.SavePreviousModalNewProject || _settings.SavePreviousModalOpenProject)
            ImGui::OpenPopup("Current project not saved");

        ImGuiExt::RenderYesNo("Current project not saved",
            [&]() {
                if (gEditor().GetProject()->GetPath().empty())
                {
                    _settings.SaveProject = true;
                }
                else
                {
                    gEditor().SaveProject(gEditor().GetProject()->GetPath());

                    if (_settings.SavePreviousModalNewProject) _settings.NewProject = true;
                    else _settings.OpenProject = true;
                }

                _settings.SavePreviousModalNewProject = false;
                _settings.SavePreviousModalOpenProject = false;
            },
            [&]() {
                if (_settings.SavePreviousModalNewProject) _settings.NewProject = true;
                else _settings.OpenProject = true;

                _settings.SavePreviousModalNewProject = false;
                _settings.SavePreviousModalOpenProject = false;
            },
            []() {},
            "Your current project is not saved. Do you want to save it before creating a new project ?"
        );
    }

    void WidgetMenuBar::NewProject()
    {
        if (gEditor().GetSettings().State == Editor::EditorState::Modified)
        {
            _settings.SavePreviousModalNewProject = true;
        }
        else
        {
            _settings.NewProject = true;
        }
    }

    void WidgetMenuBar::OpenProject()
    {
        if (gEditor().GetSettings().State == Editor::EditorState::Modified)
        {
            _settings.SavePreviousModalOpenProject = true;
        }
        else
        {
            _settings.OpenProject = true;
        }
    }

    void WidgetMenuBar::SaveProject()
    {
        if (gEditor().GetSettings().State == Editor::EditorState::Modified)
        {
            if (gEditor().GetProject()->GetPath().empty())
                _settings.SaveProject = true;
            else
                gEditor().SaveProject(gEditor().GetProject()->GetPath());
        }
    }

    void WidgetMenuBar::SaveProjectAs()
    {
        _settings.SaveProject = true;
    }

    void WidgetMenuBar::Quit()
    {
        if (gEditor().GetSettings().State == Editor::EditorState::Modified)
        {
            // TODO quit app
        }

        gCoreApplication().OnStopRequested();
    }
}
