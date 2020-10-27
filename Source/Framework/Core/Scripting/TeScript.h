#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeSceneActor.h"
#include "CoreUtility/TeCoreObject.h"
#include "TeNativeScript.h"

namespace te
{
    class TE_CORE_EXPORT Script : public CoreObject, public NonCopyable
    {
    public:
        ~Script() = default;

        /** Creates a new script. */
        static SPtr<Script> Create(const SPtr<NativeScript>& nativeScript);

        /**	Creates a new script instance without initializing it. */
        static SPtr<Script> CreateEmpty();

        /** A script can handle a native script object */
        void SetNativeScript(const SPtr<NativeScript>& nativeScript) { _nativeScript = nativeScript; }

        /** Returns native script handled */
        const SPtr<NativeScript>& GetNativeScript() const { return _nativeScript; }

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
    };
}
