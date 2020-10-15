#include "TeWidgetConsole.h"

namespace te
{
    WidgetConsole::WidgetConsole()
        : Widget(WidgetType::Console)
    { 
        _title = "Console";
    }

    WidgetConsole::~WidgetConsole()
    { }

    void WidgetConsole::Initialize()
    { }

    void WidgetConsole::Update()
    { }
}
