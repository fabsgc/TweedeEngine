#pragma once

#include "TeCorePrerequisites.h"
#include "Audio/TeAudioListener.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	AudioListener
     *
     * @note Wraps AudioListener as a Component.
     */
    class TE_CORE_EXPORT CAudioListener : public Component
    {
    public:
        virtual ~CAudioListener();

        static UINT32 GetComponentType() { return TypeID_Core::TID_CAudioListener; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HAudioListener& c, const String& suffix = "");

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { _internal->_markCoreDirty(); }

        /** @copydoc Component::Update */
        void Update() override;

        /** Returns the AudioListener implementation wrapped by this component. */
        AudioListener* GetInternal() const { return _internal.get(); }

        /** @copydoc AudioListener::GetTransform */
        const Transform& GetTransform() { return _internal->GetTransform(); }

        /** @copydoc SceneActor::GetActive */
        bool GetActive() const { return _internal != nullptr; }

    protected:
        friend class SceneObject;

        CAudioListener(const HSceneObject& parent);

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnEnabled */
        void OnDisabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    protected:
        using Component::DestroyInternal;

        /**
        * Creates the internal representation of the Animation and restores the values saved by the Component.
        * Node : clips must be added manually because this method can't do it properly
        */
        void RestoreInternal();

        /** Destroys the internal Animation representation. */
        void DestroyInternal();

        /**
         * Updates the transform of the internal AudioListener representation from the transform of the component's scene
         * object.
         */
        void UpdateTransform();

    protected:
        CAudioListener();

    protected:
        SPtr<AudioListener> _internal;
        Vector3 _lastPosition = Vector3::ZERO;
        Vector3 _velocity = Vector3::ZERO;
    };
}
