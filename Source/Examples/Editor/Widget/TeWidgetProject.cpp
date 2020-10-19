#include "TeWidgetProject.h"

namespace te
{
    WidgetProject::WidgetProject()
        : Widget(WidgetType::Project)
    { 
        _title = PROJECT_TITLE;
    }

    WidgetProject::~WidgetProject()
    { }

    void WidgetProject::Initialize()
    { }

    void WidgetProject::Update()
    { }
}
