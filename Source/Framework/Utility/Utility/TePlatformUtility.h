#pragma once

#include "Prerequisites/TePlatformDefines.h"
#include "Utility/TeUUID.h"

namespace te
{
    class PlatformUtility
    {
    public:
        /**
         * Terminates the current process.
         * @param[in]	force	True if the process should be forcefully terminated with no cleanup.
         */
        [[noreturn]] static void Terminate(bool force = false);

        /** Creates a new universally unique identifier (UUID/GUID). */
        static UUID GenerateUUID();
    };
}