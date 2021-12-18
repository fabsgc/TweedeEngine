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

        /** @copydoc PlaneCollider::SetNormal */
        void SetNormal(const Vector3& normal);

        /** @copydoc PlaneCollider::GetNormal */
        const Vector3& GetNormal() const { return _normal; }

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /** @copydoc CCollider::IsValidParent */
        bool IsValidParent(const HRigidBody& parent) const override;

        /**	Returns the plane collider that this component wraps. */
        PlaneCollider* _getInternal() const { return static_cast<PlaneCollider*>(_internal.get()); } 

    protected:
        CPlaneCollider(); // Serialization only

    protected:
        Vector3 _normal = Vector3(0.0f, 1.0f, 0.0f);
    };
}
