set (TE_SCENE_EXPORTER_INC_NOFILTER
    "TeProjectExporterPrerequisites.h"
    "TeProjectExporter.h"
)

set (TE_SCENE_EXPORTER_SRC_NOFILTER
    "TeProjectExporter.cpp"
    "TeProjectExporterPlugin.cpp"
)

source_group ("" FILES ${TE_SCENE_EXPORTER_SRC_NOFILTER} ${TE_SCENE_EXPORTER_INC_NOFILTER})

set (TE_SCENE_EXPORTER_SRC
    ${TE_SCENE_EXPORTER_INC_NOFILTER}
    ${TE_SCENE_EXPORTER_SRC_NOFILTER}
)
