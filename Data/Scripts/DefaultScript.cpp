#include "Scripting/TeNativeScript.h"

using namespace te;

extern "C" TE_PLUGIN_EXPORT NativeScript* LoadScript()
{
    return new DefaultScript();
}

class DefaultScript : public NativeScript
{
    ~DefaultScript()
    { }

    virtual void Update() override
    { }
};
