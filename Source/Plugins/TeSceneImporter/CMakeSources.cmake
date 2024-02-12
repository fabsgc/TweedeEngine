set (TE_SCENE_IMPORTER_INC_NOFILTER
    "TeSceneImporterPrerequisites.h"
    "TeSceneImporter.h"
)

set (TE_SCENE_IMPORTER_SRC_NOFILTER
    "TeSceneImporter.cpp"
    "TeSceneImporterPlugin.cpp"
)

source_group ("" FILES ${TE_SCENE_IMPORTER_SRC_NOFILTER} ${TE_SCENE_IMPORTER_INC_NOFILTER})

set (TE_SCENE_IMPORTER_SRC
    ${TE_SCENE_IMPORTER_INC_NOFILTER}
    ${TE_SCENE_IMPORTER_SRC_NOFILTER}
)
