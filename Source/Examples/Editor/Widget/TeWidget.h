#pragma once

#include "TeCorePrerequisites.h"
#include "TeCoreApplication.h"
#include "Math/TeVector2.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "../ImGuiExt/TeImGuiExt.h"
#include "../ImGuiExt/TeIconsFontAwesome5.h"

#include <cfloat>

#define CONSOLE_TITLE ICON_FA_TERMINAL " Console"
#define MENUBAR_TITLE "MenuBar"
#define PROJECT_TITLE ICON_FA_BONG " Project"
#define PROPERTIES_TITLE ICON_FA_TOOLS " Properties"
#define RENDER_OPTIONS_TITLE ICON_FA_COG " Render Options"
#define RESOURCES_TITLE ICON_FA_SUITCASE " Resources"
#define TOOLBAR_TITLE "ToolBar"
#define VIEWPORT_TITLE ICON_FA_IMAGE " Viewport"
#define SCRIPT_TITLE ICON_FA_SCROLL " Script"
#define MATERIALS_TITLE ICON_FA_PALETTE " Materials"
#define PROFILER_TITLE ICON_FA_CHART_LINE " Profiler"
#define SETTINGS_TITLE ICON_FA_COGS " Settings"
#define SHADERS_TITLE ICON_FA_PAINT_BRUSH " Shaders"

namespace te
{
    class Widget
    {
    public:
        enum class IconType
        {
            NotAssigned,
            ComponentOptions
        };

        enum class WidgetType
        {
            Console,
            MenuBar,
            Project,
            Properties,
            RenderOptions,
            ToolBar,
            Viewport,
            Resources,
            Script,
            Material,
            Profiler,
            Settings,
            Shaders,
            None
        };

    public:
        Widget(WidgetType type);
        virtual ~Widget() = default;

        /** Initialize Widget */
        virtual void Initialize() = 0;

        /** Update Widget */
        virtual void Update() = 0;

        /** Update Widget while in background */
        virtual void UpdateBackground() = 0;

        /** Give focus to the current widget */
        virtual void PutFocus();

        /** Initialize ImGui for this Widget */
        bool BeginGui();

        /** End ImGui for this Widget */
        bool EndGui();

        bool IsWindow() const { return _isWindow; }
        float GetHeight() const { return _height; }
        bool GetVisible() const { return _isVisible; }
        void SetVisible(bool isVisible) { _isVisible = isVisible; }
        const auto& GetTitle() const { return _title; }
        const auto& GetTitleActivated() const { return _titleActivated; }
        WidgetType GetType() const { return _type; }
        ImGuiWindow* GetWindow() const { return _window; }
        ImGuiViewport* GetViewport() const { return _viewport; }

    protected:
        bool _isVisible;
        bool _isWindow;
        bool _begun;
        WidgetType _type;
        UINT32 _flags = ImGuiWindowFlags_NoCollapse;
        float _height = 0.0f;
        float _alpha = -1.0f;
        Vector2 _position = Vector2(-1.0f, -1.0f);
        Vector2 _size = Vector2(-1.0f, -1.0f);
        Vector2 _sizeMax = Vector2(FLT_MAX, FLT_MAX);
        Vector2 _padding = Vector2(-1.0f, - 1.0f);
        std::function<void()> _onStartCallback = nullptr;
        std::function<void()> _onVisibleCallback = nullptr;
        std::function<void()> _onBeginCallback = nullptr;

        String _title;
        String _titleActivated;

        ImGuiWindow* _window = nullptr;
        ImGuiViewport* _viewport = nullptr;

        UINT8 _varPushes = 0;
    };
}
