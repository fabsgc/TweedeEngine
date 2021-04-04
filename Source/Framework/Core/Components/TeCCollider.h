#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Physics/TeCollider.h"

namespace te
{
    /**
     * @copydoc	Collider
     *
     * @note Wraps Collider as a Component.
     */
    class TE_CORE_EXPORT CCollider : public Component
    {
    public:
        CCollider(const HSceneObject& parent, UINT32 type);
        virtual ~CCollider();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CCollider; }

        /** @copydoc Component::Initialize */
        virtual void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HCollider& c);

        /** @copydoc Component::update */
        void Update() override { }

    protected:
        friend class SceneObject;
        using Component::DestroyInternal;

        /** @copydoc Component::onInitialized() */
        void OnInitialized() override;

        /** @copydoc Component::onDestroyed() */
        void OnDestroyed() override;

        /** @copydoc Component::onDisabled() */
        void OnDisabled() override;

        /** @copydoc Component::onEnabled() */
        void OnEnabled() override;

        /** @copydoc Component::onTransformChanged() */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** Creates the internal representation of the Joint for use by the component. */
        virtual SPtr<Collider> CreateInternal() = 0;

        /** Creates the internal representation of the Joint and restores the values saved by the Component. */
        virtual void RestoreInternal();

        /** Destroys the internal joint representation. */
        virtual void DestroyInternal();

    protected:
        CCollider(UINT32 type);

    protected:
        SPtr<Collider> _internal;
    };
}
