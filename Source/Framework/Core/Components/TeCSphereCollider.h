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
        

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CSphereCollider; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HSphereCollider& c, const String& suffix = "");

        /** @copydoc SphereCollider::SetRadius */
        void SetRadius(float radius);

        /** @copydoc SphereCollider::GetRadius */
        float GetRadius() const { return _radius; }

    protected:
        friend class SceneObject;

        CSphereCollider(); // Serialization only
        CSphereCollider(const HSceneObject& parent, float radius = 1.0f);

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the box collider that this component wraps. */
        SphereCollider* GetInternal() const { return static_cast<SphereCollider*>(_internal.get()); }

    protected:
        float _radius = 1.0f;
    };
}
