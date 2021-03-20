#pragma once

#include "TeCorePrerequisites.h"
#include "Scripting/TeScript.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    struct ScriptIdentifier;

    /**
     * @copydoc	Script
     *
     * @note	Wraps Script as a Component.
     */
    class TE_CORE_EXPORT CScript : public Component
    {
    public:
        CScript(const HSceneObject& parent);
        virtual ~CScript();

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Script::SetNativeScript */
        void SetNativeScript(const String& name, const String& path = "") { _internal->SetNativeScript(name, SO(), path); }

        /** @copydoc Script::SetNativeScript */
        void SetNativeScript(const ScriptIdentifier& identifier) { _internal->SetNativeScript(identifier, SO()); }

        /** @copydoc Script::GetNativeScript */
        const NativeScript* GetNativeScript() { return _internal->GetNativeScript(); }

        /** @copydoc Script::GetNativeScriptName */
        const String GetNativeScriptName() const { return _internal->GetNativeScriptName(); }

        /** @copydoc Script::GetNativeScriptPath */
        const String GetNativeScriptPath() const { return _internal->GetNativeScriptPath(); }

        /** Returns the internal renderable that is used for majority of operations by this component. */
        SPtr<Script> _getInternal() const { return _internal; }

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CScript; }

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HScript& c);

    protected:
        mutable SPtr<Script> _internal;

    protected:
        friend class SceneObject;

        /** @copydoc Component::_instantiate */
        void _instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnEnabled */
        void OnDisabled() override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;
    
    public:
        /** @copydoc Component::Update */
        void Update() override;

    protected:
        CScript();
    };
}
