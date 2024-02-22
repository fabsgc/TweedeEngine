set (TE_PROJECT_IMPORTER_INC_NOFILTER
    "TeProjectImporterPrerequisites.h"
    "TeProjectImporter.h"
)

set (TE_PROJECT_IMPORTER_SRC_NOFILTER
    "TeProjectImporter.cpp"
    "TeProjectImporterPlugin.cpp"
)

source_group ("" FILES ${TE_PROJECT_IMPORTER_SRC_NOFILTER} ${TE_PROJECT_IMPORTER_INC_NOFILTER})

set (TE_PROJECT_IMPORTER_SRC
    ${TE_PROJECT_IMPORTER_INC_NOFILTER}
    ${TE_PROJECT_IMPORTER_SRC_NOFILTER}
)
