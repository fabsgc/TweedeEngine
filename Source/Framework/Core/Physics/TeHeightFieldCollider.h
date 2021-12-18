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
        HeightFieldCollider();
        ~HeightFieldCollider() = default;

        /**
         * Sets a texture that represents the vertical position of the terrain
         */
        virtual void SetHeightField(const HPhysicsHeightField& heightField) { _heightField = heightField; }

        /** @copydoc SetHeightField */
        HPhysicsHeightField GetHeightField() const { return _heightField; }

        /**
         * Specify the minimum height of this terrain
         */
        virtual void SetMinHeight(const float& minHeight) { _minHeight = minHeight; }

        /** @copydoc SetMinHeight */
        float GetMinHeight() { return _minHeight; }

        /**
         * Specify the maximum height of this terrain
         */
        virtual void SetMaxHeight(const float& maxHeight) { _maxHeight = maxHeight; }

        /** @copydoc SetMinHeight */
        float GetMaxHeight() { return _maxHeight; }

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
        float _minHeight = 0.0f;
        float _maxHeight = 10.0f;
    };
}
