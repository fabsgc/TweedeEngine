#include "Scripting/TeNativeScript.h"

#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

using namespace te;

class DefaultScript : public NativeScript
{
public:
    DefaultScript()
        : NativeScript()
    { }

    ~DefaultScript()
    { }

    virtual void OnStartup() override
    { }

    virtual void OnShutdown() override
    { }

    virtual void Update() override
    {
        // TE_PRINT("DEFAULT");
    }
};

extern "C" TE_SCRIPT_EXPORT NativeScript* LoadScript()
{
    return te_new<DefaultScript>();
}

extern "C" TE_SCRIPT_EXPORT void UnloadScript(NativeScript* script)
{
    if(script) te_delete(script);
}
