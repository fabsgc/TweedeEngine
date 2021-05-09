#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    class FPhysicsHeightField;

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
        static UINT32 GetResourceType() { return TID_PhysicsHeightField; }

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
        static SPtr<PhysicsHeightField> _createPtr(const SPtr<Texture>& texture);

        /** Returns the internal implementation of the physics height field. */
        virtual FPhysicsHeightField* _getInternal() { return _internal.get(); }

    protected:
        SPtr<FPhysicsHeightField> _internal;
        SPtr<Texture> _initTexture; // Transient, only used during initalization
    };

    /** Foundation that contains a specific implementation of a PhysicsHeightField. */
    class TE_CORE_EXPORT FPhysicsHeightField : public Serializable
    {
    public:
        FPhysicsHeightField(const SPtr<Texture>& texture, UINT32 TID_type);
        virtual ~FPhysicsHeightField() = default;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_FPhysicsHeightField; }

        /** @copydoc PhysicsHeightField::GetTexture */
        SPtr<Texture> GetTexture() const { return _texture;  }

    protected:
        friend class PhysicsHeightField;

        SPtr<Texture> _texture = nullptr;
    };
}
