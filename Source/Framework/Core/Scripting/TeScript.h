#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeSceneActor.h"
#include "CoreUtility/TeCoreObject.h"
#include "TeNativeScript.h"

namespace te
{
    enum class ScriptState
    {
        Enabled, Disabled
    };

    class TE_CORE_EXPORT Script : public CoreObject, public NonCopyable
    {
    public:
        ~Script();

        /** Creates a new script with script name in parameter. */
        static SPtr<Script> Create(const String& name);

        /**	Creates a new script instance without initializing it. */
        static SPtr<Script> CreateEmpty();

        /** A script can handle a native script object */
        void SetNativeScript(const String& name);

        /** Returns native script handled */
        const SPtr<NativeScript>& GetNativeScript() const { return _nativeScript; }

        /** Called when creating a new script */
        void OnStartup();

        /** Called before destroying this script */
        void OnShutdown();

        /** Called this script is disabled */
        void OnDisabled();

        /** Called this script is enabled */
        void OnEnabled();

        /** Called before scene update */
        void PreUpdate();

        /** Called during scene update (sceneObject arbitrary order) */
        void Update();

        /** Called after scene update */
        void PostUpdate();

        /** Called after engine render*/
        void PostRender();

    protected:
        friend class CScript;

        Script();
        Script(const SPtr<NativeScript>& nativeScript);

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

        /** @copydoc CoreObject::initialize */
        void Initialize() override;

    protected:
        SPtr<NativeScript> _nativeScript;
        ScriptState _state;
    };
}
