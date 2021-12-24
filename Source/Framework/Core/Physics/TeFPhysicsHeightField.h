#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    /** Foundation that contains a specific implementation of a PhysicsHeightField. */
    class TE_CORE_EXPORT FPhysicsHeightField : public Serializable
    {
    public:
        explicit FPhysicsHeightField(const SPtr<Texture>& texture);
        virtual ~FPhysicsHeightField() = default;

        /** @copydoc PhysicsHeightField::GetTexture */
        SPtr<Texture> GetTexture() const { return _texture;  }

    protected:
        SPtr<Texture> _texture = nullptr;
    };
}
