#pragma once

#include "TeCorePrerequisites.h"
#include "Components/TeCSoftBody.h"
#include "Physics/TeRopeSoftBody.h"

namespace te
{
    /**
     * @copydoc	RopeSoftBody
     *
     * @note Wraps RopeSoftBody as a Component.
     */
    class TE_CORE_EXPORT CRopeSoftBody : public CSoftBody
    {
    public:
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CRopeSoftBody; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent & c, const String & suffix = "");
        
        /** @copydoc Component::Clone */
        bool Clone(const HRopeSoftBody& c, const String& suffix = "");

    protected:
        friend class SceneObject;

        CRopeSoftBody(); // Serialization only
        explicit CRopeSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;
    };
}
