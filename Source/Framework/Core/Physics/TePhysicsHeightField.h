#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    /**
     * Represents a HeightField built from a square height field texture that can be used with a HeightFieldCollider.
     */
    class TE_CORE_EXPORT PhysicsHeightField : public Resource
    {
    public:
        PhysicsHeightField(const SPtr<Texture>& texture);
        virtual ~PhysicsHeightField() = default;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TypeID_Core::TID_PhysicsHeightField; }

        /** Returns the original texture used for height field generation. */
        SPtr<Texture> GetTexture() const;

        /**
         * Creates a new physics height field.
         *
         * @param[in]	texture		Texture used for height field generation.
         */
        static HPhysicsHeightField Create(const SPtr<Texture>& texture);

        /**
         * @copydoc Create()
         */
        static HPhysicsHeightField Create(const HTexture& texture);

        /**
         * @copydoc Create()
         *
         * For internal use. Requires manual initialization after creation.
         */
        static SPtr<PhysicsHeightField> CreatePtr(const SPtr<Texture>& texture);

        /** Returns the internal implementation of the physics height field. */
        virtual FPhysicsHeightField* GetInternal() { return _internal.get(); }

    protected:
        SPtr<FPhysicsHeightField> _internal;
        SPtr<Texture> _initTexture; // Transient, only used during initalization
    };
}
