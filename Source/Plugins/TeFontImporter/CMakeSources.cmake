set (TE_FONTIMPORTER_INC_NOFILTER
    "TeFontImporterPrerequisites.h"
    "TeFontImporter.h"
)

set (TE_FONTIMPORTER_SRC_NOFILTER
    "TeFontImporter.cpp"
    "TeFontImporterPlugin.cpp"
)

source_group ("" FILES ${TE_FONTIMPORTER_SRC_NOFILTER} ${TE_FONTIMPORTER_INC_NOFILTER})

set (TE_FONTIMPORTER_SRC
    ${TE_FONTIMPORTER_INC_NOFILTER}
    ${TE_FONTIMPORTER_SRC_NOFILTER}
)
