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
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CCylinderCollider; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HCylinderCollider& c, const String& suffix = "");

        /** @copydoc CylinderCollider::SetExtents */
        void SetExtents(const Vector3& extents);

        /** @copydoc CylinderCollider::GetExtents */
        Vector3 GetExtents() const { return _extents; }

    protected:
        friend class SceneObject;

        CCylinderCollider(); // Serialization only
        CCylinderCollider(const HSceneObject& parent);

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the cylinder collider that this component wraps. */
        CylinderCollider* GetInternal() const { return static_cast<CylinderCollider*>(_internal.get()); }

    protected:
        Vector3 _extents = Vector3(0.5f, 0.5f, 0.5f);
    };
}
