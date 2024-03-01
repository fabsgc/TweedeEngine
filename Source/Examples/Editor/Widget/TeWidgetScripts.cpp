#include "TeWidgetScripts.h"

namespace te
{
    WidgetScripts::WidgetScripts()
        : Widget(WidgetType::Scripts)
    { 
        _title = SCRIPTS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetScripts::~WidgetScripts()
    { }

    void WidgetScripts::Initialize()
    { }

    void WidgetScripts::Update()
    { }

    void WidgetScripts::UpdateBackground()
    { }
}
