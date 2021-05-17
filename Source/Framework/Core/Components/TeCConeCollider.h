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

        /** @copydoc CapsuleCollider::SetRadius */
        void SetRadius(float radius);

        /** @copydoc CapsuleCollider::GetRadius */
        float GetRadius() const { return _radius; }

        /** @copydoc CapsuleCollider::SetHeight */
        void SetHeight(float height);

        /** @copydoc CapsuleCollider::GetHeight */
        float GetHeight() const { return _height; }

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the cone collider that this component wraps. */
        ConeCollider* _getInternal() const { return static_cast<ConeCollider*>(_internal.get()); }

    protected:
        CConeCollider(); // Serialization only

    protected:
        float _radius = 1.0f;
        float _height = 1.0f;
    };
}
