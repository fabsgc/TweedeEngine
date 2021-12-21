#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeBoxCollider.h"
#include "Components/TeCCollider.h"
#include "Math/TeVector3.h"

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
        CBoxCollider(const HSceneObject& parent, const Vector3& extents = Vector3(0.5f, 0.5f, 0.5f));

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CBoxCollider; }

        /** @copydoc Component::Clone */
        bool Clone(const HBoxCollider& c, const String& suffix = "");

        /** @copydoc BoxCollider::SetExtents */
        void SetExtents(const Vector3& extents);

        /** @copydoc BoxCollider::GetExtents */
        Vector3 GetExtents() const { return _extents; }

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the box collider that this component wraps. */
        BoxCollider* GetInternal() const { return static_cast<BoxCollider*>(_internal.get()); }

    protected:
        CBoxCollider(); // Serialization only

    protected:
        Vector3 _extents = Vector3(0.5f, 0.5f, 0.5f);
    };
}
