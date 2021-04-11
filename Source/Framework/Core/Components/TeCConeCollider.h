#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeConeCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	ConeCollider
     *
     * @note	Wraps ConeCollider as a Component.
     */
    class TE_CORE_EXPORT CConeCollider : public CCollider
    {
    public:
        CConeCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CConeCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HConeCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /**	Returns the cone collider that this component wraps. */
        ConeCollider* _getInternal() const { return static_cast<ConeCollider*>(_internal.get()); }

    protected:
        CConeCollider(); // Serialization only
    };
}
