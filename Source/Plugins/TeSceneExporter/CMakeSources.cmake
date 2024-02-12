set (TE_SCENE_EXPORTER_INC_NOFILTER
    "TeSceneExporterPrerequisites.h"
    "TeSceneExporter.h"
)

set (TE_SCENE_EXPORTER_SRC_NOFILTER
    "TeSceneExporter.cpp"
    "TeSceneExporterPlugin.cpp"
)

source_group ("" FILES ${TE_SCENE_EXPORTER_SRC_NOFILTER} ${TE_SCENE_EXPORTER_INC_NOFILTER})

set (TE_SCENE_EXPORTER_SRC
    ${TE_SCENE_EXPORTER_INC_NOFILTER}
    ${TE_SCENE_EXPORTER_SRC_NOFILTER}
)
