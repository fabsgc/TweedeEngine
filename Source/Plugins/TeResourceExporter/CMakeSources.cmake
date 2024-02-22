set (TE_RESOURCE_EXPORTER_INC_NOFILTER
    "TeResourceExporterPrerequisites.h"
    "TeResourceExporter.h"
)

set (TE_RESOURCE_EXPORTER_SRC_NOFILTER
    "TeResourceExporter.cpp"
    "TeResourceExporterPlugin.cpp"
)

source_group ("" FILES ${TE_RESOURCE_EXPORTER_SRC_NOFILTER} ${TE_RESOURCE_EXPORTER_INC_NOFILTER})

set (TE_RESOURCE_EXPORTER_SRC
    ${TE_RESOURCE_EXPORTER_INC_NOFILTER}
    ${TE_RESOURCE_EXPORTER_SRC_NOFILTER}
)
