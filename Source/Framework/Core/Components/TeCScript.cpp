#include "Components/TeCScript.h"

#include "Scene/TeSceneManager.h"
#include "Scripting/TeScript.h"
#include "Scripting/TeScriptManager.h"
#include "TeCoreApplication.h"

namespace te
{
    CScript::CScript()
        : Component(HSceneObject(), (UINT32)TID_CScript)
    {
        SetName("Script");
        SetNotifyFlags(TCF_Parent);
        SetFlag(Component::AlwaysRun, true);
    }

    CScript::CScript(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CScript)
    {
        SetName("Script");
        SetNotifyFlags(TCF_Parent);
        SetFlag(Component::AlwaysRun, true);
    }

    CScript::~CScript()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CScript::Instantiate()
    {
        _internal = Script::Create(String(), SO());
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
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (gScriptManager().IsPaused() || !isRunning)
            return;

        _internal->Update();
    }

    bool CScript::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CScript>(c), suffix);
    }

    bool CScript::Clone(const HScript& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            _internal = Script::Create(c->GetNativeScriptName(), SO());
            return true;
        }

        return false;
    }
}
