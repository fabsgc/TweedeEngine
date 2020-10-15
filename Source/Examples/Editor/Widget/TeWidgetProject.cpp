#include "TeWidgetProject.h"

namespace te
{
    WidgetProject::WidgetProject()
        : Widget(WidgetType::Project)
    { 
        _title = "Project";
    }

    WidgetProject::~WidgetProject()
    { }

    void WidgetProject::Initialize()
    { }

    void WidgetProject::Update()
    { }
}
