#include "TeWidgetToolBar.h"

#include "../TeEditor.h"
#include "Math/TeMath.h"
#include "TeWidgetRenderOptions.h"
#include "Animation/TeAnimationManager.h"
#include "Scripting/TeScriptManager.h"
#include "Manager/TeRenderDocManager.h"
#include "Scripting/TeScript.h"
#include "Scene/TeSceneManager.h"
#include "Components/TeCScript.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Renderer/TeRenderer.h"
#include "Components/TeCLight.h"
#include "Exporter/TeExporter.h"
#include "Exporter/TeTextureExportOptions.h"

namespace te
{
    WidgetToolBar::WidgetToolBar()
        : Widget(WidgetType::ToolBar)
    { 
        _title = TOOLBAR_TITLE;
    }

    WidgetToolBar::~WidgetToolBar()
    { }

    void WidgetToolBar::Initialize()
    {
        _flags =
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoTitleBar;

        _onVisibleCallback = [this]()
        {
            float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);

            auto& ctx = *ImGui::GetCurrentContext();
            ctx.NextWindowData.MenuBarOffsetMinVal = ImVec2(ctx.Style.DisplaySafeAreaPadding.x, Math::Max(ctx.Style.DisplaySafeAreaPadding.y - ctx.Style.FramePadding.y, 0.0f));
            _position = Vector2(ctx.Viewports[0]->Pos.x, ctx.Viewports[0]->Pos.y + 25.0f);
            _size = Vector2(width, ctx.NextWindowData.MenuBarOffsetMinVal.y + ctx.FontBaseSize + ctx.Style.FramePadding.y + 18.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));
        };

        _onBeginCallback = [this]()
        {
            ImGui::PopStyleVar();
        };
    }

    void WidgetToolBar::Update()
    {
        auto ShowButton = [this](const char* titleEnabled, const char* titleDisabled, const std::function<bool()>& getVisibility, const std::function<void()>& makeVisible, const String& tooltip = String())
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, getVisibility() ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);
            if (ImGui::Button(getVisibility() ? titleEnabled : titleDisabled))
            {
                makeVisible();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !tooltip.empty())
            {
                ImGui::SetTooltip(tooltip.c_str());
            }
            ImGui::PopStyleColor();
        };

        // Play
        ShowButton(ICON_FA_PLAY, ICON_FA_STOP,
            [this]() {
                return !gCoreApplication().GetState().IsFlagSet(ApplicationState::Game); 
            },
            [this]() {
                Vector<HComponent> components = gEditor().GetSceneRoot()->GetComponents<CScript>(true);
                for (auto& component : components)
                {
                    HScript handle = static_object_cast<CScript>(component);
                    SPtr<Script> script = handle->GetInternal();

                    if (script != nullptr)
                    {
                        if (gCoreApplication().GetState().IsFlagSet(ApplicationState::Game))
                            script->OnShutdown();
                        else
                            script->OnStartup();
                    }
                }

                gScriptManager().TogglePaused();

                gCoreApplication().GetState().ToggleFlag(ApplicationState::Mode::Game);
                gCoreApplication().GetState().ToggleFlag(ApplicationState::Mode::Physics);
                gCoreApplication().GetState().ToggleFlag(ApplicationState::Mode::Scripting);
                gCoreApplication().GetState().ToggleFlag(ApplicationState::Mode::Animation);

                bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Mode::Game);
                if (isRunning)
                {
                    gEditor().CreateRunningScene();
                }
                else
                {
                    gEditor().DestroyRunningScene();
                }
            }
        );

        // Guizmo translation
        ShowButton(ICON_FA_ARROWS_ALT, ICON_FA_ARROWS_ALT,
            [this]() { return !(gEditor().GetImGuizmoOperation() == ImGuizmo::OPERATION::TRANSLATE); },
            [this]() { gEditor().SetImGuizmoOperation(ImGuizmo::OPERATION::TRANSLATE); }
        );

        // Guizmo rotation
        ShowButton(ICON_FA_SYNC, ICON_FA_SYNC,
            [this]() { return !(gEditor().GetImGuizmoOperation() == ImGuizmo::OPERATION::ROTATE); },
            [this]() { gEditor().SetImGuizmoOperation(ImGuizmo::OPERATION::ROTATE); }
        );

        // Guizmo scale
        ShowButton(ICON_FA_COMPRESS_ALT, ICON_FA_COMPRESS_ALT,
            [this]() { return !(gEditor().GetImGuizmoOperation() == ImGuizmo::OPERATION::SCALE); },
            [this]() { gEditor().SetImGuizmoOperation(ImGuizmo::OPERATION::SCALE); }
        );

        // Guizmo mode
        static ImGuiExt::ComboOptions<ImGuizmo::MODE> guizmoModeOptions;
        if (guizmoModeOptions.Options.size() == 0)
        {
            guizmoModeOptions.AddOption(ImGuizmo::MODE::LOCAL, "Local Space");
            guizmoModeOptions.AddOption(ImGuizmo::MODE::WORLD, "World Space");
        }

        ImGuizmo::MODE guizmoMode = gEditor().GetImGuizmoMode();

        ImGui::SameLine();
        if (ImGuiExt::RenderOptionCombo<ImGuizmo::MODE>((ImGuizmo::MODE*)(&guizmoMode), "##guizmo_mode_option", "", guizmoModeOptions, 150))
            gEditor().SetImGuizmoMode(guizmoMode);

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        // RenderDoc
        ShowButton(ICON_FA_BUG, ICON_FA_BUG,
            [this]() { return false; },
            [this]() { 
                gEditor().NeedsRedraw();
                RenderDocManager::Instance().FrameCapture();
            },
            "Capture the next frame and then launch RenderDoc"
        );
#endif

        // Save all textures generated by the renderer to the disk
        ShowButton(ICON_FA_CAMERA_RETRO, ICON_FA_CAMERA_RETRO,
            [this]() { return false; },
            [this]() {
                struct TextureElement
                {
                    SPtr<Texture> texture;
                    SPtr<TextureExportOptions> option;
                };

                gEditor().NeedsRedraw();
                Vector<TextureElement> textures;

                for (int i = 0; i != static_cast<int>(RenderOutputType::Count); i++)
                {
                    RenderOutputType renderType = static_cast<RenderOutputType>(i);
                    SPtr<Texture> texture = gRenderer()->GetLastRenderTexture(renderType);

                    if (texture)
                    {
                        TextureElement textureElement;

                        textureElement.texture = texture;
                        textureElement.option = TextureExportOptions::Create();

                        if (renderType == RenderOutputType::Depth)
                        {
                            textureElement.option->IsDepthStencilBuffer = true;
                            textureElement.option->IsSingleChannel = true;
                        }

                        if (renderType == RenderOutputType::SSAO)
                        {
                            textureElement.option->IsSingleChannel = true;
                        }

                        textures.push_back(textureElement);
                    }
                }

                Vector<HComponent> lights = gEditor().GetSceneRoot()->GetComponents(TID_CLight, true);
                for(const auto& light : lights)
                {
                    SPtr<Texture> texture = gRenderer()->GetLastShadowMapTexture(static_object_cast<CLight>(light).GetInternalPtr()->GetInternal());

                    if (texture)
                    {
                        TextureElement textureElement;

                        textureElement.texture = texture;
                        textureElement.option = TextureExportOptions::Create();

                        textures.push_back(textureElement);
                    }
                }

                UINT32 count = 0;
                for (const auto& texture : textures)
                {
                    gExporter().Export(texture.texture.get(), ToString(count) + ".jpeg", texture.option);
                    count++;
                }
            },
            "Save all textures generated by the renderer to the disk"
        );
    }

    void WidgetToolBar::UpdateBackground()
    {
    }
}
