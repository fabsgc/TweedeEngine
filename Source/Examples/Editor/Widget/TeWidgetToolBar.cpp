#include "TeWidgetToolBar.h"

#include "Math/TeMath.h"
#include "ImGui/imgui.h"
#include "../TeEditor.h"
#include "ImGui/imgui_internal.h"

#include "TeWidgetRenderOptions.h"

namespace te
{
    WidgetToolBar::WidgetToolBar()
        : Widget(WidgetType::ToolBar)
    { 
        _title = "ToolBar";
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
            auto& ctx = *ImGui::GetCurrentContext();
            ctx.NextWindowData.MenuBarOffsetMinVal = ImVec2(ctx.Style.DisplaySafeAreaPadding.x, Math::Max(ctx.Style.DisplaySafeAreaPadding.y - ctx.Style.FramePadding.y, 0.0f));
            _position = Vector2(ctx.Viewports[0]->Pos.x, ctx.Viewports[0]->Pos.y + 22.0f);
            _size = Vector2(ctx.Viewports[0]->Size.x, ctx.NextWindowData.MenuBarOffsetMinVal.y + ctx.FontBaseSize + ctx.Style.FramePadding.y + 20.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));
        };

        _onBeginCallback = [this]()
        {
            ImGui::PopStyleVar();
        };

        WidgetRenderOptions* widget = static_cast<WidgetRenderOptions*>(Editor::Instance().GetWidget(WidgetType::RenderOptions));
        _widgets[Widget::IconType::ComponentOptions] = te_shared_ptr<WidgetRenderOptions>(widget);
    }

    void WidgetToolBar::Update()
    {
        auto showButton = [this](const char* title, const std::function<bool()>& getVisibility, const std::function<void()>& makeVisible)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, getVisibility() ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);
            if (ImGui::Button(title, ImVec2(120.0f, _buttonSize)))
            {
                makeVisible();
            }
            ImGui::PopStyleColor();
        };

        for (auto& widgetPair : _widgets)
        {
            const IconType icon = widgetPair.first;
            SPtr<Widget> widget = widgetPair.second;

            showButton(widget->GetTitle().c_str(), [this, &widget]() { return widget->GetVisible(); }, [this, &widget]() { widget->SetVisible(true); });
        }
    }
}
