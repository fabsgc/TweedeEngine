#include "TeWidgetToolBar.h"

#include "../TeIconsFontAwesome5.h"
#include "../TeEditor.h"
#include "Math/TeMath.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "TeWidgetRenderOptions.h"

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

        //WidgetRenderOptions* widget = static_cast<WidgetRenderOptions*>(Editor::Instance().GetWidget(WidgetType::RenderOptions));
        //_widgets[Widget::IconType::ComponentOptions] = te_shared_ptr<WidgetRenderOptions>(widget);
    }

    void WidgetToolBar::Update()
    {
        auto ShowButton = [this](const char* titleEnabled, const char* titleDisabled, const std::function<bool()>& getVisibility, const std::function<void()>& makeVisible)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, getVisibility() ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);
            if (ImGui::Button(getVisibility() ? titleEnabled : titleDisabled))
            {
                makeVisible();
            }
            ImGui::PopStyleColor();
        };

        ShowButton(ICON_FA_PLAY, ICON_FA_STOP,
            [this]() { return !gCoreApplication().GetState().IsFlagSet(ApplicationState::Game); },
            [this]() { gCoreApplication().GetState().ToggleFlag(ApplicationState::Game); }
        );

        ShowButton(ICON_FA_ARROWS_ALT, ICON_FA_ARROWS_ALT,
            [this]() { return true; },
            [this]() { return; }
        );

        ShowButton(ICON_FA_SYNC, ICON_FA_SYNC,
            [this]() { return true; },
            [this]() { return; }
        );

        ShowButton(ICON_FA_COMPRESS_ALT, ICON_FA_COMPRESS_ALT,
            [this]() { return true; },
            [this]() { return; }
        );

        for (auto& widgetPair : _widgets)
        {
            SPtr<Widget> widget = widgetPair.second;

            ShowButton(widget->GetTitle().c_str(), widget->GetTitleActivated().c_str(), 
                [this, &widget]() { return widget->GetVisible(); }, 
                [this, &widget]() { widget->SetVisible(true); 
            });
        }
    }

    void WidgetToolBar::UpdateBackground()
    { }
}
