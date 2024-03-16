#include "Scripting/TeNativeScript.h"

#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

using namespace te;

class FirstScript : public NativeScript
{
public:
    FirstScript()
        : NativeScript()
    { }

    ~FirstScript()
    { }

    virtual void OnStartup() override
    {
        TE_PRINT("FirstScript::OnStartup");
    }

    virtual void OnShutdown() override
    {
        TE_PRINT("FirstScript::OnShutdown");
    }

    virtual void Update() override
    {
        TE_PRINT("FirstScript::Update");
    }
};

extern "C" TE_SCRIPT_EXPORT NativeScript* LoadScript()
{
    return te_new<FirstScript>();
}

extern "C" TE_SCRIPT_EXPORT void UnloadScript(NativeScript* script)
{
    if(script) te_delete(script);
}
