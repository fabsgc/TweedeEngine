#pragma once

#include "Prerequisites/TePlatformDefines.h"
#include "Utility/TeUUID.h"

namespace te
{
    /** Contains information about available GPUs on the system. */
    struct GPUInfo
    {
        String Names[5];
        UINT32 NumGPUs;
    };

    /** Contains information about the system hardware and operating system. */
    struct SystemInfo
    {
        String CpuManufacturer;
        String CpuModel;
        UINT32 CpuClockSpeedMhz;
        UINT32 CpuNumCores;
        UINT32 MemoryAmountMb;
        String OsName;
        bool OsIs64Bit;

        GPUInfo GpuInfo;
    };

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