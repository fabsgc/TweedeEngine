#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSphereCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	SphereCollider
     *
     * @note	Wraps SphereCollider as a Component.
     */
    class TE_CORE_EXPORT CSphereCollider : public CCollider
    {
    public:
        CSphereCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CSphereCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HSphereCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Collider> CreateInternal() override;

    protected:
        CSphereCollider(); // Serialization only
    };
}
