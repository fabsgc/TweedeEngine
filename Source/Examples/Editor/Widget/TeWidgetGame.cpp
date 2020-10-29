#include "TeWidgetGame.h"
#include "../TeEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "TeCoreApplication.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    WidgetGame::WidgetGame()
        : Widget(WidgetType::Game)
    {
        _title = GAME_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove;
    }

    WidgetGame::~WidgetGame()
    { }

    void WidgetGame::Initialize()
    { }

    void WidgetGame::Update()
    { }

    void WidgetGame::UpdateBackground()
    { }
}
