#include "TeWidgetShaders.h"

#include "../TeEditor.h"
#include "Math/TeMath.h"
#include "TeWidgetRenderOptions.h"
#include "Animation/TeAnimationManager.h"
#include "Scripting/TeScriptManager.h"
#include "Manager/TeRenderDocManager.h"
#include "Scripting/TeScript.h"
#include "Scene/TeSceneManager.h"
#include "Components/TeCScript.h"
#include "ImGuizmo/ImGuizmo.h"

namespace te
{
    WidgetShaders::WidgetShaders()
        : Widget(WidgetType::Shaders)
    { 
        _title = SHADERS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetShaders::~WidgetShaders()
    { }

    void WidgetShaders::Initialize()
    {
        _onVisibleCallback = [this]()
        {
            
        };

        _onBeginCallback = [this]()
        {

        };
    }

    void WidgetShaders::Update()
    {

    }

    void WidgetShaders::UpdateBackground()
    { }
}
