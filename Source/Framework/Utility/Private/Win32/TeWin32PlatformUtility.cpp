#include "Prerequisites/TePrerequisitesUtility.h"
#include "Utility/TePlatformUtility.h"
#include <windows.h>
#include <iphlpapi.h>
#include <intrin.h>

#if defined(TE_WIN_SDK_10) && defined(_MSC_VER)
#   include <Rpc.h>
#   pragma comment(lib, "Rpcrt4.lib")
#endif

namespace te
{
    void PlatformUtility::Terminate(bool force)
	{
		if (!force)
			PostQuitMessage(0);
		else
			TerminateProcess(GetCurrentProcess(), 0);
	}

    UUID PlatformUtility::GenerateUUID()
    {
        ::UUID uuid;
        UuidCreate(&uuid);

        // Endianess might not be correct, but it shouldn't matter
        UINT32 data1 = uuid.Data1;
        UINT32 data2 = uuid.Data2 | (uuid.Data3 << 16);
        UINT32 data3 = uuid.Data3 | (uuid.Data4[0] << 16) | (uuid.Data4[1] << 24);
        UINT32 data4 = uuid.Data4[2] | (uuid.Data4[3] << 8) | (uuid.Data4[4] << 16) | (uuid.Data4[5] << 24);

        return UUID(data1, data2, data3, data4);
    }
}