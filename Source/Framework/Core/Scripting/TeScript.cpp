#include "Scripting/TeScript.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    Script::Script()
        : Resource(TID_Script)
    { }

    HScript Script::Create()
    {
        SPtr<Script> newScript = CreatePtr();
        return static_resource_cast<Script>(gResourceManager()._createResourceHandle(newScript));
    }

    SPtr<Script> Script::CreatePtr()
    {
        SPtr<Script> newScript = te_core_ptr<Script>(new (te_allocate<Script>()) Script());
        newScript->SetThisPtr(newScript);
        newScript->Initialize();

        return newScript;
    }

    SPtr<Script> Script::CreateEmpty()
    {
        SPtr<Script> newScript = te_core_ptr<Script>(new (te_allocate<Script>()) Script());
        newScript->SetThisPtr(newScript);

        return newScript;
    }
}
