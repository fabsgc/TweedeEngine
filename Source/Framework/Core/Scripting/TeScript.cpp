#include "TeScript.h"
#include "TeNativeScript.h"

namespace te
{
    Script::Script()
    { }

    Script::Script(const SPtr<NativeScript>& nativeScript)
        : _nativeScript(nativeScript)
    { }

    SPtr<Script> Script::CreateEmpty()
    {
        Script* script = new (te_allocate<Script>()) Script();
        SPtr<Script> scriptPtr = te_core_ptr<Script>(script);
        scriptPtr->SetThisPtr(scriptPtr);

        return scriptPtr;
    }

    SPtr<Script> Script::Create(const SPtr<NativeScript>& nativeScript)
    {
        SPtr<Script> scriptPtr = CreateEmpty();
        scriptPtr->Initialize();
        scriptPtr->SetNativeScript(nativeScript);

        return scriptPtr;
    }

    void Script::Initialize()
    {
        CoreObject::Initialize();
    }

    void Script::FrameSync()
    { }
}
