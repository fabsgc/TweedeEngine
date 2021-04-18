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
        CSphereCollider(const HSceneObject& parent, float radius = 1.0f);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CSphereCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HSphereCollider& c);

        /** @copydoc SphereCollider::SetRadius() */
        void SetRadius(float radius);

        /** @copydoc SphereCollider::GetRadius() */
        float GetRadius() const { return _radius; }

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the box collider that this component wraps. */
        SphereCollider* _getInternal() const { return static_cast<SphereCollider*>(_internal.get()); }

    protected:
        CSphereCollider(); // Serialization only

    protected:
        float _radius = 1.0f;
    };
}
