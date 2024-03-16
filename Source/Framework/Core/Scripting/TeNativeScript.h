#pragma once

#include "TeCorePrerequisites.h"
#include "Serialization/TeSerializable.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    /**
     * A native script is the base class of all scripts that can be written, loaded and executed inside the engine
     */
    class TE_CORE_EXPORT NativeScript : public Serializable, public NonCopyable
    {
    public:
        NativeScript();
        virtual ~NativeScript() = 0;

        /** 
         * Called when a new script using this native script is created
         */
        virtual void OnStartup() { }

        /**
         * Called when a script using this native script is destroyed (before)
         */
        virtual void OnShutdown() { }

        /**
         * Called before scene update
         */
        virtual void PreUpdate() { }

        /**
         * Called during scene update (sceneObject arbitrary order)
         */
        virtual void Update() { }

        /**
         * Called after scene update
         */
        virtual void PostUpdate() { }

        /**
         * Called after engine render
         */
        virtual void PostRender() { }

    public:
        /**
         * Because a script lives inside the scene graph we want to store an handle to its parent SceneObject
         */
        virtual void SetParentSceneObject(const HSceneObject& sceneObject) final
        { 
            if (!sceneObject.Empty())
                _parentSO = sceneObject.GetNewHandleFromExisting();
            else
                _parentSO = HSceneObject();
        }

        /**
         * @copydoc SetParentSceneObject
         */
        virtual const HSceneObject& GetParentSceneObject() const final { return _parentSO; }

    private:
        HSceneObject _parentSO;
    };
}
