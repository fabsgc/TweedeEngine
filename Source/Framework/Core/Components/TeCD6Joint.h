#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeD6Joint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	D6Joint
     *
     * @note	Wraps D6Joint as a Component.
     */
    class TE_CORE_EXPORT CD6Joint : public CJoint
    {
    public:
        CD6Joint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CD6Joint; }

        /** @copydoc Component::Clone */
        void Clone(const HD6Joint& c);

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the 6Dof joint that this component wraps. */
        D6Joint* GetInternal() const { return static_cast<D6Joint*>(_internal.get()); }

    protected:
        CD6Joint(); // Serialization only
    };
}
