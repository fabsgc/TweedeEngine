#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeBoxCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	BoxCollider
     *
     * @note	Wraps BoxCollider as a Component.
     */
    class TE_CORE_EXPORT CBoxCollider : public CCollider
    {
    public:
        CBoxCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CBoxCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HBoxCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Collider> CreateInternal() override;

    protected:
        CBoxCollider(); // Serialization only
    };
}
