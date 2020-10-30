set (TE_EDITOR_INC_NOFILTER
    "TeEditor.h"
    "TeApplication.h"
    "TeEditorResManager.h"
)

set (TE_EDITOR_SRC_NOFILTER
    "Main.cpp"
    "TeEditor.cpp"
    "TeApplication.cpp"
    "TeEditorResManager.cpp"
)

set (TE_IMGUI_EXT_INC_NOFILTER
    "ImGuiExt/TeImGuiExt.h"
    "ImGuiExt/TeImGuiFileDialog.h"
    "ImGuiExt/TeIconsFontAwesome5.h"
)

set (TE_IMGUI_EXT_SRC_NOFILTER
    "ImGuiExt/TeImGuiExt.cpp"
    "ImGuiExt/TeImGuiFileDialog.cpp"
)


set (TE_EDITOR_INC_WIDGET
    "Widget/TeWidget.h"
    "Widget/TeWidgetMenuBar.h"
    "Widget/TeWidgetToolBar.h"
    "Widget/TeWidgetConsole.h"
    "Widget/TeWidgetProject.h"
    "Widget/TeWidgetViewport.h"
    "Widget/TeWidgetProperties.h"
    "Widget/TeWidgetRenderOptions.h"
    "Widget/TeWidgetResources.h"
    "Widget/TeWidgetScript.h"
    "Widget/TeWidgetMaterials.h"
)

set (TE_EDITOR_SRC_WIDGET
    "Widget/TeWidget.cpp"
    "Widget/TeWidgetMenuBar.cpp"
    "Widget/TeWidgetToolBar.cpp"
    "Widget/TeWidgetConsole.cpp"
    "Widget/TeWidgetProject.cpp"
    "Widget/TeWidgetViewport.cpp"
    "Widget/TeWidgetProperties.cpp"
    "Widget/TeWidgetRenderOptions.cpp"
    "Widget/TeWidgetResources.cpp"
    "Widget/TeWidgetScript.cpp"
    "Widget/TeWidgetMaterials.cpp"
)

source_group ("" FILES ${TE_EDITOR_SRC_NOFILTER} ${TE_EDITOR_INC_NOFILTER})
source_group ("Widget" FILES ${TE_EDITOR_INC_WIDGET} ${TE_EDITOR_SRC_WIDGET})
source_group ("ImGuiExt" FILES ${TE_IMGUI_EXT_INC_NOFILTER} ${TE_IMGUI_EXT_SRC_NOFILTER})

set (TE_EDITOR_SRC
    ${TE_EDITOR_INC_NOFILTER}
    ${TE_EDITOR_SRC_NOFILTER}
    ${TE_EDITOR_INC_WIDGET}
    ${TE_EDITOR_SRC_WIDGET}
    ${TE_IMGUI_EXT_INC_NOFILTER}
    ${TE_IMGUI_EXT_SRC_NOFILTER}
)
