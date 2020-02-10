set (TE_SPONZA_INC_NOFILTER
    "TeApplication.h"
)

set (TE_SPONZA_SRC_NOFILTER
    "Main.cpp"
    "TeApplication.cpp"
)

source_group ("" FILES ${TE_SPONZA_SRC_NOFILTER} ${TE_SPONZA_INC_NOFILTER})

set (TE_SPONZA_SRC
    ${TE_SPONZA_INC_NOFILTER}
    ${TE_SPONZA_SRC_NOFILTER}
)
