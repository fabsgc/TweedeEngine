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
    "ImGuiExt/TeImGuiTextEditor.h"
    "ImGuiExt/TeIconsFontAwesome5.h"
    "ImGuiExt/dirent.h"
)

set (TE_EDITOR_SRC_IMGUI_EXT
    "ImGuiExt/TeImGuiExt.cpp"
    "ImGuiExt/TeImGuiFileBrowser.cpp"
    "ImGuiExt/TeImGuiTextEditor.cpp"
)

set (TE_EDITOR_INC_SELECTION
    "Selection/TeSelection.h"
    "Selection/TeSelectionMat.h"
    "Selection/TeHudSelectionMat.h"
    "Selection/TeHudPickingMat.h"
    "Selection/TeHud.h"
    "Selection/TeEditorPicking.h"
)

set (TE_EDITOR_SRC_SELECTION
    "Selection/TeSelection.cpp"
    "Selection/TeSelectionMat.cpp"
    "Selection/TeHudSelectionMat.cpp"
    "Selection/TeHudPickingMat.cpp"
    "Selection/TeHud.cpp"
    "Selection/TeEditorPicking.cpp"
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
    "Widget/TeWidgetProfiler.h"
    "Widget/TeWidgetSettings.h"
    "Widget/TeMaterialsPreview.h"
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
    "Widget/TeWidgetProfiler.cpp"
    "Widget/TeWidgetSettings.cpp"
    "Widget/TeMaterialsPreview.cpp"
)

source_group ("" FILES ${TE_EDITOR_SRC_NOFILTER} ${TE_EDITOR_INC_NOFILTER})
source_group ("Widget" FILES ${TE_EDITOR_INC_WIDGET} ${TE_EDITOR_SRC_WIDGET})
source_group ("ImGuiExt" FILES ${TE_EDITOR_INC_IMGUI_EXT} ${TE_EDITOR_SRC_IMGUI_EXT})
source_group ("Selection" FILES ${TE_EDITOR_INC_SELECTION} ${TE_EDITOR_SRC_SELECTION})

set (TE_EDITOR_SRC
    ${TE_EDITOR_INC_NOFILTER}
    ${TE_EDITOR_SRC_NOFILTER}
    ${TE_EDITOR_INC_WIDGET}
    ${TE_EDITOR_SRC_WIDGET}
    ${TE_EDITOR_INC_IMGUI_EXT}
    ${TE_EDITOR_SRC_IMGUI_EXT}
    ${TE_EDITOR_INC_SELECTION}
    ${TE_EDITOR_SRC_SELECTION}
)
