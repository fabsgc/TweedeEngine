#pragma once

#include "TeCorePrerequisites.h"

namespace te
{ 
    /** Determines what parent, if any, owns a physics object. */
    enum class PhysicsOwnerType
    {
        None, /** No parent, object is used directly. */
        Component, /** Object is used by a C++ Component. */
        Script /** Object is used by the scripting system. */
    };

    /** Contains information about a parent for a physics object. */
    struct PhysicsObjectOwner
    {
        PhysicsOwnerType Type = PhysicsOwnerType::None; /**< Type of owner. */
    };
}
