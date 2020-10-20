#include "TeWidgetRenderOptions.h"

namespace te
{
    WidgetRenderOptions::WidgetRenderOptions()
        : Widget(WidgetType::RenderOptions)
    { 
        _title = RENDER_OPTIONS_TITLE;
    }

    WidgetRenderOptions::~WidgetRenderOptions()
    { }

    void WidgetRenderOptions::Initialize()
    { }

    void WidgetRenderOptions::Update()
    { }

    void WidgetRenderOptions::UpdateBackground()
    { }
}
