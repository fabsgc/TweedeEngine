#include "Scripting/TeNativeScript.h"

#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

using namespace te;

class ExampleScript : public NativeScript
{
public:
    ExampleScript()
        : NativeScript()
    { }

    ~ExampleScript()
    { }

    virtual void OnStartup() override
    { }

    virtual void OnShutdown() override
    { }

    virtual void Update() override
    { }
};

extern "C" TE_SCRIPT_EXPORT NativeScript* LoadScript()
{
    return te_new<ExampleScript>();
}

extern "C" TE_SCRIPT_EXPORT void UnloadScript(NativeScript* script)
{
    if(script) te_delete(script);
}
