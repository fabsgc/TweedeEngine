#include "Utility/TePlatformUtility.h"
#include <uuid/uuid.h>

namespace te
{
    void PlatformUtility::Terminate(bool force)
    {
        exit(0);
    }

    UUID PlatformUtility::GenerateUUID()
    {
        uuid_t nativeUUID;
        uuid_generate(nativeUUID);

        return UUID(
            *(UINT32*)&nativeUUID[0],
            *(UINT32*)&nativeUUID[4],
            *(UINT32*)&nativeUUID[8],
            *(UINT32*)&nativeUUID[12]);
    }
}
