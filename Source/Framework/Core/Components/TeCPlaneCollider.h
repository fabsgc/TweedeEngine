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
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CPlaneCollider; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HPlaneCollider& c, const String& suffix = "");

        /** @copydoc PlaneCollider::SetNormal */
        void SetNormal(const Vector3& normal);

        /** @copydoc PlaneCollider::GetNormal */
        const Vector3& GetNormal() const { return _normal; }

    protected:
        friend class SceneObject;

        CPlaneCollider(); // Serialization only
        CPlaneCollider(const HSceneObject& parent);

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /** @copydoc CCollider::IsValidParent */
        bool IsValidParent(const HRigidBody& parent) const override;

        /**	Returns the plane collider that this component wraps. */
        PlaneCollider* GetInternal() const { return static_cast<PlaneCollider*>(_internal.get()); } 

    protected:
        Vector3 _normal = Vector3(0.0f, 1.0f, 0.0f);
    };
}
