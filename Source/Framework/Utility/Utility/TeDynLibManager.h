#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Utility/TeModule.h"

namespace te
{
    class DynLib;

    /**
     * This manager keeps track of all the open dynamic-loading libraries, it manages opening them opens them and can be
     * used to lookup already already-open libraries.
     *
     * @note Not thread safe.
     */
    class TE_UTILITY_EXPORT DynLibManager : public Module<DynLibManager>
    {
    public:
        DynLib* Load(String name);
        void Unload(DynLib* lib);

        TE_MODULE_STATIC_HEADER_MEMBER(DynLibManager)

    protected:
        Set<UPtr<DynLib>, std::less<>> _loadedLibrairies;
    };

    /** Easy way of accessing DynLibManager. */
    TE_UTILITY_EXPORT DynLibManager& gDynLibManager();
}
