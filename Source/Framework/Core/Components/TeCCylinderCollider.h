#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCylinderCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	CylinderCollider
     *
     * @note	Wraps CylinderCollider as a Component.
     */
    class TE_CORE_EXPORT CCylinderCollider : public CCollider
    {
    public:
        CCylinderCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CCylinderCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HCylinderCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Collider> CreateInternal() override;

    protected:
        CCylinderCollider(); // Serialization only
    };
}
