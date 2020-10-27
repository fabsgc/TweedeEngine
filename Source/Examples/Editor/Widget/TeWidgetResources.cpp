#include "TeWidgetResources.h"

namespace te
{
    WidgetResources::WidgetResources()
        : Widget(WidgetType::Resources)
    { 
        _title = RESOURCES_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetResources::~WidgetResources()
    { }

    void WidgetResources::Initialize()
    { }

    void WidgetResources::Update()
    { }

    void WidgetResources::UpdateBackground()
    { }
}
