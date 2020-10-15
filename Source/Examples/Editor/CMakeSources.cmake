set (TE_EDITOR_INC_NOFILTER
    "TeEditor.h"
    "TeApplication.h"
)

set (TE_EDITOR_SRC_NOFILTER
    "Main.cpp"
    "TeEditor.cpp"
    "TeApplication.cpp"
)

set (TE_EDITOR_INC_WIDGET
    "Widget/TeWidget.h"
    "Widget/TeWidgetMenuBar.h"
)

set (TE_EDITOR_SRC_WIDGET
    "Widget/TeWidget.cpp"
    "Widget/TeWidgetMenuBar.cpp"
)

source_group ("" FILES ${TE_EDITOR_SRC_NOFILTER} ${TE_EDITOR_INC_NOFILTER})
source_group ("Widget" FILES ${TE_EDITOR_INC_WIDGET} ${TE_EDITOR_SRC_WIDGET})

set (TE_EDITOR_SRC
    ${TE_EDITOR_INC_NOFILTER}
    ${TE_EDITOR_SRC_NOFILTER}
    ${TE_EDITOR_INC_WIDGET}
    ${TE_EDITOR_SRC_WIDGET}
)
