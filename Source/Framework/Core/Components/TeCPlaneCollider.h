#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePlaneCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	PlaneCollider
     *
     * @note	Wraps PlaneCollider as a Component.
     */
    class TE_CORE_EXPORT CPlaneCollider : public CCollider
    {
    public:
        CPlaneCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CPlaneCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HPlaneCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Collider> CreateInternal() override;

    protected:
        CPlaneCollider(); // Serialization only
    };
}
