set (TE_BULLETPHYSICS_INC_NOFILTER
    "TeBulletPhysics.h"
    "TeBulletPhysicsPrerequisites.h"
)

set (TE_BULLETPHYSICS_SRC_NOFILTER
    "TeBulletPhysicsPlugin.cpp"
    "TeBulletPhysics.cpp"
)

source_group ("" FILES ${TE_BULLETPHYSICS_SRC_NOFILTER} ${TE_BULLETPHYSICS_INC_NOFILTER})

set (TE_BULLETPHYSICS_SRC
    ${TE_BULLETPHYSICS_INC_NOFILTER}
    ${TE_BULLETPHYSICS_SRC_NOFILTER}
)
