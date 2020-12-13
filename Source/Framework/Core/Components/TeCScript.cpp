#include "Components/TeCScript.h"
#include "Scene/TeSceneManager.h"
#include "Scripting/TeScript.h"

namespace te
{
    CScript::CScript()
        : Component(HSceneObject(), (UINT32)TID_CScript)
    {
        SetName("Script");
        SetFlag(Component::AlwaysRun, true);
    }

    CScript::CScript(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CScript)
    {
        SetName("Script");
        SetFlag(Component::AlwaysRun, true);
    }

    CScript::~CScript()
    {
        if (!_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CScript::_instantiate()
    {
        _internal = Script::Create(String(), SO());
    }

    void CScript::Initialize()
    {
        Component::Initialize();
    }

    void CScript::OnInitialized()
    {
        _internal->OnStartup();
        Component::OnInitialized();
    }

    void CScript::OnEnabled()
    {
        _internal->OnEnabled();
        Component::OnEnabled();
    }

    void CScript::OnDisabled()
    {
        _internal->OnDisabled();
        Component::OnDisabled();
    }

    void CScript::OnDestroyed()
    {
        _internal->OnShutdown();
        Component::OnDestroyed();
    }

    void CScript::Update()
    {
        _internal->Update();
    }

    void CScript::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CScript>(c));
    }

    void CScript::Clone(const HScript& c)
    {
        Component::Clone(c.GetInternalPtr());
        _internal = Script::Create(c->GetNativeScriptName(), SO());
    }
}
