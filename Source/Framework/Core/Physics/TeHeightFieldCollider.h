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
         * Sets a mesh that represents the collider geometry. This can be a generic triangle mesh, or and convex mesh.
         */
        void SetHeightField(const HPhysicsHeightField& heightField) { _heightField = _heightField; OnHeightFieldChanged(); }

        /** @copydoc SetHeightField() */
        HPhysicsHeightField GetHeightField() const { return _heightField; }

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
    };
}
