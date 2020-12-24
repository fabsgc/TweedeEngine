#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(DynLibManager)

    static void dynlib_delete(DynLib* lib)
    {
        lib->Unload();
        te_delete(lib);
    }

    static bool operator<(const UPtr<DynLib>& lhs, const String& rhs)
    {
        return lhs->GetName() < rhs;
    }

    static bool operator<(const String& lhs, const UPtr<DynLib>& rhs)
    {
        return lhs < rhs->GetName();
    }

    static bool operator<(const UPtr<DynLib>& lhs, const UPtr<DynLib>& rhs)
    {
        return lhs->GetName() < rhs->GetName();
    }

    DynLib* DynLibManager::Load(String filename)
    {
        // Add the extension (.dll, .so, ...) if necessary.

        // Note: The string comparison here could be slightly more efficent by using a templatized string_concat function
        // for the lower_bound call and/or a custom comparitor that does comparison by parts.
        const String::size_type length = filename.length();
        const String extension = String(".") + DynLib::EXTENSION;
        const String::size_type extLength = extension.length();
        if (length <= extLength || filename.substr(length - extLength) != extension)
            filename.append(extension);

        if (DynLib::PREFIX != nullptr)
            filename.insert(0, DynLib::PREFIX);

        const auto& iterFind = _loadedLibrairies.lower_bound(filename);
        if (iterFind != _loadedLibrairies.end() && (*iterFind)->GetName() == filename)
        {
            return iterFind->get();
        }
        else
        {
            DynLib* newLib = te_new<DynLib>(std::move(filename));
             _loadedLibrairies.emplace_hint(iterFind, newLib);
            return newLib;
        }
    }

    void DynLibManager::Unload(DynLib* lib)
    {
        const auto& iterFind = _loadedLibrairies.find(lib->GetName());
        if (iterFind != _loadedLibrairies.end())
        {
            iterFind->get()->Unload();
            _loadedLibrairies.erase(iterFind);
        }
        else
        {
            // Somehow a DynLib not owned by the manager...?
            // Well, we should clean it up anyway...
            dynlib_delete(lib);
        }
    }

    DynLibManager& gDynLibManager()
    {
        return DynLibManager::Instance();
    }
}
