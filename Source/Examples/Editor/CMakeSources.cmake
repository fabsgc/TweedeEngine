set (TE_EDITOR_INC_NOFILTER
    "TeEditor.h"
    "TeApplication.h"
    "TeIconsFontAwesome5.h"
    "TeImGuiExt.h"
)

set (TE_EDITOR_SRC_NOFILTER
    "Main.cpp"
    "TeEditor.cpp"
    "TeApplication.cpp"
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
)

source_group ("" FILES ${TE_EDITOR_SRC_NOFILTER} ${TE_EDITOR_INC_NOFILTER})
source_group ("Widget" FILES ${TE_EDITOR_INC_WIDGET} ${TE_EDITOR_SRC_WIDGET})

set (TE_EDITOR_SRC
    ${TE_EDITOR_INC_NOFILTER}
    ${TE_EDITOR_SRC_NOFILTER}
    ${TE_EDITOR_INC_WIDGET}
    ${TE_EDITOR_SRC_WIDGET}
)
