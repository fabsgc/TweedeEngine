set (TE_RESOURCE_IMPORTER_INC_NOFILTER
    "TeResourceImporterPrerequisites.h"
    "TeResourceImporter.h"
)

set (TE_RESOURCE_IMPORTER_SRC_NOFILTER
    "TeResourceImporter.cpp"
    "TeResourceImporterPlugin.cpp"
)

source_group ("" FILES ${TE_RESOURCE_IMPORTER_SRC_NOFILTER} ${TE_RESOURCE_IMPORTER_INC_NOFILTER})

set (TE_RESOURCE_IMPORTER_SRC
    ${TE_RESOURCE_IMPORTER_INC_NOFILTER}
    ${TE_RESOURCE_IMPORTER_SRC_NOFILTER}
)
