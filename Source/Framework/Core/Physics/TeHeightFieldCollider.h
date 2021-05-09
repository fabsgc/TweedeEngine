#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeCollider.h"

namespace te
{
    class PhysicsScene;

    /** Collider with HeightField geometry. */
    class TE_CORE_EXPORT HeightFieldCollider : public Collider
    {
    public:
        HeightFieldCollider() = default;
        ~HeightFieldCollider() = default;

        /**
         * Sets a texture that represents the vertical position of the terrain
         */
        void SetHeightField(const HPhysicsHeightField& heightField) { _heightField = _heightField; OnHeightFieldChanged(); }

        /** @copydoc SetHeightField() */
        HPhysicsHeightField GetHeightField() const { return _heightField; }

        /**
         * Specify the scale you want to apply to the terrain (texture value are between 0 and 1)
         */
        void SetHeightScale(const float& heightScale) { _heightScale = heightScale; OnHeightFieldChanged(); }

        /** @copydoc SetHeightScale */
        float GetHeightScale() { return _heightScale; }

        /**
         * Creates a new HeightField collider.
         *
         * @param[in]	scene		Scene into which to add the collider to.
         * @param[in]	position	Position of the collider relative to its parent
         * @param[in]	rotation	Position of the collider relative to its parent
         */
        static SPtr<HeightFieldCollider> Create(PhysicsScene& scene, const Vector3& position = Vector3::ZERO,
            const Quaternion& rotation = Quaternion::IDENTITY);

    protected:
        /**
         * Triggered by the resources system whenever the attached height field changed (e.g. was reimported) or loaded.
         */
        virtual void OnHeightFieldChanged() { }

    protected:
        HPhysicsHeightField _heightField;
        float _heightScale = 1.0f;
    };
}
