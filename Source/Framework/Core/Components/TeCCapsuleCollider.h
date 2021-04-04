#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCapsuleCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	CapsuleCollider
     *
     * @note	Wraps CapsuleCollider as a Component.
     */
    class TE_CORE_EXPORT CCapsuleCollider : public CCollider
    {
    public:
        CCapsuleCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CCapsuleCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HCapsuleCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Collider> CreateInternal() override;

    protected:
        CCapsuleCollider(); // Serialization only
    };
}
