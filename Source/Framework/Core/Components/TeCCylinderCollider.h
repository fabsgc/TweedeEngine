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

        /** @copydoc CylinderCollider::SetExtents() */
        void SetExtents(const Vector3& extents);

        /** @copydoc CylinderCollider::GetExtents() */
        Vector3 GetExtents() const { return _extents; }

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /**	Returns the cylinder collider that this component wraps. */
        CylinderCollider* _getInternal() const { return static_cast<CylinderCollider*>(_internal.get()); }

    protected:
        CCylinderCollider(); // Serialization only

    protected:
        Vector3 _extents = Vector3(0.5f, 0.5f, 0.5f);
    };
}
