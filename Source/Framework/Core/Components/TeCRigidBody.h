#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	Rigidbody
     *
     * @note Wraps RigidBody as a Component.
     */
    class TE_CORE_EXPORT CRigidBody : public Component
    {
    public:
        CRigidBody(const HSceneObject& parent);
        ~CRigidBody();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CRigidBody; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HRigidBody& c);

        /** @copydoc Component::update */
        void Update() override { }

        /** Returns the Rigidbody implementation wrapped by this component. */
        RigidBody* GetInternal() const { return _internal.get(); }

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

        /** Destroys the internal RigidBody representation. */
        virtual void DestroyInternal();

    protected:
        CRigidBody(); // Serialization only

    protected:
        SPtr<RigidBody> _internal;

    };
}
