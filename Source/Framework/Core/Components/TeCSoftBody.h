#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSoftBody.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	SoftBody
     *
     * @note Wraps SoftBody as a Component.
     */
    class TE_CORE_EXPORT CSoftBody : public Component
    {
    public:
        CSoftBody(const HSceneObject& parent);
        ~CSoftBody();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CSoftBody; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HSoftBody& c);

        /** @copydoc Component::update */
        void Update() override { }

        /** Returns the SoftBody implementation wrapped by this component. */
        SoftBody* GetInternal() const { return _internal.get(); }

    protected:
        friend class SceneObject;
        using Component::DestroyInternal;

        /** @copydoc Component::OnInitialized() */
        void OnInitialized() override;

        /** @copydoc Component::OnDestroyed() */
        void OnDestroyed() override;

        /** @copydoc Component::OnDisabled() */
        void OnDisabled() override;

        /** @copydoc Component::OnEnabled() */
        void OnEnabled() override;

        /** @copydoc Component::OnTransformChanged() */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** Destroys the internal SoftBody representation. */
        virtual void DestroyInternal();

    protected:
        CSoftBody(); // Serialization only

    protected:
        SPtr<SoftBody> _internal;

    };
}
