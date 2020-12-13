#pragma once

#include "TeCorePrerequisites.h"
#include "Scripting/TeScript.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneObject.h"

namespace te
{
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
        void SetNativeScript(const String& name) { _internal->SetNativeScript(name, SO()); }

        /** @copydoc Script::GetNativeScript */
        const NativeScript* GetNativeScript() { return _internal->GetNativeScript(); }

        /** @copydoc Script::GetNativeScriptName */
        const String GetNativeScriptName() const { return _internal->GetNativeScriptName(); }

        /** Returns the internal renderable that is used for majority of operations by this component. */
        SPtr<Script> _getInternal() const { return _internal; }

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
