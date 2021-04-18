#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeHeightFieldCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	HeightFieldCollider
     *
     * @note	Wraps HeightFieldCollider as a Component.
     */
    class TE_CORE_EXPORT CHeightFieldCollider : public CCollider
    {
    public:
        CHeightFieldCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CHeightFieldCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HHeightFieldCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the HeightField collider that this component wraps. */
        HeightFieldCollider* _getInternal() const { return static_cast<HeightFieldCollider*>(_internal.get()); }

    protected:
        CHeightFieldCollider(); // Serialization only
    };
}
