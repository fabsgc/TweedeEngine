#pragma once

#include "TeCorePrerequisites.h"
#include "Animation/TeAnimation.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	Animation
     *
     * @note	Wraps Animation as a Component.
     */
    class TE_CORE_EXPORT CAnimation : public Component
    {
    public:
        CAnimation(const HSceneObject& parent);
        virtual ~CAnimation() = default;

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HAnimation& c);

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { _internal->MarkCoreDirty(); }

        /** @copydoc Component::update */
        void Update() override { }

        /** Returns the Animation implementation wrapped by this component. */
        SPtr<Animation> _getInternal() const { return _internal; }

    protected:
        friend class SceneObject;

        /** @copydoc Component::_instantiate */
        void _instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnCreated() override { }

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnEnabled */
        void OnDisabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override { }

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    protected:
        CAnimation();

    protected:
        SPtr<Animation> _internal = nullptr;
    };
}
