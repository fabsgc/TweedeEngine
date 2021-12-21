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
        static UINT32 GetComponentType() { return TypeID_Core::TID_CCapsuleCollider; }

        /** @copydoc Component::Clone */
        bool Clone(const HCapsuleCollider& c, const String& suffix = "");

        /** @copydoc CapsuleCollider::SetRadius */
        void SetRadius(float radius);

        /** @copydoc CapsuleCollider::GetRadius */
        float GetRadius() const { return _radius;  }

        /** @copydoc CapsuleCollider::SetHeight */
        void SetHeight(float height);

        /** @copydoc CapsuleCollider::GetHeight */
        float GetHeight() const { return _height;  }

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the capsule collider that this component wraps. */
        CapsuleCollider* GetInternal() const { return static_cast<CapsuleCollider*>(_internal.get()); }

    protected:
        CCapsuleCollider(); // Serialization only

    protected:
        float _radius = 1.0f;
        float _height = 1.0f;
    };
}
