#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    /**
     * Script resource.
     */
    class TE_CORE_EXPORT Script : public Resource
    {
    public:
        virtual ~Script() = default;

        /** @copydoc Resource::SetPath */
        void SetPath(const String& path) override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TypeID_Core::TID_Script; }

        /**	Creates a new script from the provided per-size font data. */
        static HScript Create();

        /** Creates a new script as a pointer instead of a resource handle. */
        static SPtr<Script> CreatePtr();

        /** Creates a script without initializing it. */
        static SPtr<Script> CreateEmpty();

    private:
        Script();

    };
}
