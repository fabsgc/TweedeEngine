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
        

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CHeightFieldCollider; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HHeightFieldCollider& c, const String& suffix = "");

        /** @copydoc HeightFieldCollider::SetHeightField */
        void SetHeightField(const HPhysicsHeightField& heightField);

        /** @copydoc HeightFieldCollider::GetHeightField */
        HPhysicsHeightField GetHeightField() const { return _heightField; }

        /** @copydoc HeightFieldCollider::SetMinHeight */
        void SetMinHeight(const float& minHeight);

        /** @copydoc HeightFieldCollider::GetMinHeight */
        float GetMinHeight() const { return _minHeight; }

        /** @copydoc HeightFieldCollider::SetMaxHeight */
        void SetMaxHeight(const float& maxHeight);

        /** @copydoc HeightFieldCollider::GetMaxHeight */
        float GetMaxHeight() const { return _maxHeight; }

    protected:
        friend class SceneObject;

        CHeightFieldCollider(); // Serialization only
        CHeightFieldCollider(const HSceneObject& parent);

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the HeightField collider that this component wraps. */
        HeightFieldCollider* GetInternal() const { return static_cast<HeightFieldCollider*>(_internal.get()); }

    protected:
        HPhysicsHeightField _heightField;
        float _minHeight = 0.0f;
        float _maxHeight = 10.0f;
    };
}
