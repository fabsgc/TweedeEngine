#include "TeD3D11RenderAPIPrerequisites.h"

namespace te
{
    class D3D11Utility
    {
    public:
        template<class T>
        static void SetDebugName(T* resource, const char* name, size_t length)
        {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            String fullName = "[" + ToString(D3D11Utility::NextId) + "] " + name;
            length = fullName.size();

            if (length > 0 && resource)
                resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)(sizeof(char) * length), fullName.c_str());

            D3D11Utility::NextId++;
#endif
        }

    private:
        static UINT32 NextId;
    };
}
