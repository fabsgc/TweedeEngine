set (TE_EDITOR_INC_NOFILTER
    "TeEditor.h"
    "TeApplication.h"
    "TeEditorResManager.h"
    "TeEditorUtils.h"
)

set (TE_EDITOR_SRC_NOFILTER
    "Main.cpp"
    "TeEditor.cpp"
    "TeApplication.cpp"
    "TeEditorResManager.cpp"
    "TeEditorUtils.cpp"
)

set (TE_EDITOR_INC_IMGUI_EXT
    "ImGuiExt/TeImGuiExt.h"
    "ImGuiExt/TeImGuiFileBrowser.h"
    "ImGuiExt/TeIconsFontAwesome5.h"
    "ImGuiExt/dirent.h"
)

set (TE_EDITOR_SRC_IMGUI_EXT
    "ImGuiExt/TeImGuiExt.cpp"
    "ImGuiExt/TeImGuiFileBrowser.cpp"
)

set (TE_EDITOR_INC_PICKING
    "Picking/TeGpuPicking.h"
    "Picking/TeGpuPickingMat.h"
)

set (TE_EDITOR_SRC_PICKING
    "Picking/TeGpuPicking.cpp"
    "Picking/TeGpuPickingMat.cpp"
)

set (TE_EDITOR_INC_HUD
    "Hud/TeHud.h"
    "Hud/TeHudMat.h"
)

set (TE_EDITOR_SRC_HUD
    "Hud/TeHud.cpp"
    "Hud/TeHudMat.cpp"
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
source_group ("ImGuiExt" FILES ${TE_EDITOR_INC_IMGUI_EXT} ${TE_EDITOR_SRC_IMGUI_EXT})
source_group ("Picking" FILES ${TE_EDITOR_INC_PICKING} ${TE_EDITOR_SRC_PICKING})
source_group ("Hud" FILES ${TE_EDITOR_INC_HUD} ${TE_EDITOR_SRC_HUD})

set (TE_EDITOR_SRC
    ${TE_EDITOR_INC_NOFILTER}
    ${TE_EDITOR_SRC_NOFILTER}
    ${TE_EDITOR_INC_WIDGET}
    ${TE_EDITOR_SRC_WIDGET}
    ${TE_EDITOR_INC_IMGUI_EXT}
    ${TE_EDITOR_SRC_IMGUI_EXT}
    ${TE_EDITOR_INC_PICKING}
    ${TE_EDITOR_SRC_PICKING}
    ${TE_EDITOR_INC_HUD}
    ${TE_EDITOR_SRC_HUD}
)
