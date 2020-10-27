#include "Components/TeCScript.h"
#include "Scene/TeSceneManager.h"
#include "Scripting/TeScript.h"

namespace te
{
    CScript::CScript()
        : Component(HSceneObject(), (UINT32)TID_CScript)
    {
        SetName("Script");
    }

    CScript::CScript(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CScript)
    {
        SetName("Script");
    }

    CScript::~CScript()
    {
        if (!_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CScript::_instantiate()
    {
        _internal = Script::Create(nullptr);
    }

    void CScript::Initialize()
    {
        Component::Initialize();
    }

    void CScript::OnInitialized()
    { }

    void CScript::OnDestroyed()
    { }

    void CScript::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CScript>(c));
    }

    void CScript::Clone(const HScript& c)
    {
        Component::Clone(c.GetInternalPtr());
        SPtr<Script> script = c->_getInternal();

        // TODO : Native script copy
    }
}
