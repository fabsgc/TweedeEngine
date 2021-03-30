#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Physics/TeJoint.h"

namespace te
{
    /**
     * @copydoc	Joint
     *
     * @note Wraps Joint as a Component.
     */
    class TE_CORE_EXPORT CJoint : public Component
    {
    public:
        CJoint(const HSceneObject& parent, JOINT_DESC desc);
        virtual ~CJoint();

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CJoint; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HJoint& c);

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
        virtual SPtr<Joint> CreateInternal() = 0;

        /** Creates the internal representation of the Joint and restores the values saved by the Component. */
        virtual void RestoreInternal();

        /** Destroys the internal joint representation. */
        virtual void DestroyInternal();

    protected:
        CJoint();

    protected:
        SPtr<Joint> _internal;
        
    private:
        JOINT_DESC& _desc;
    };
}
