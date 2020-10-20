#include "TeWidgetProperties.h"

namespace te
{
    WidgetProperties::WidgetProperties()
        : Widget(WidgetType::Properties)
    { 
        _title = PROPERTIES_TITLE;
    }

    WidgetProperties::~WidgetProperties()
    { }

    void WidgetProperties::Initialize()
    { }

    void WidgetProperties::Update()
    { }

    void WidgetProperties::UpdateBackground()
    { }
}
