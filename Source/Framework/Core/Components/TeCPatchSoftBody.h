#pragma once

#include "TeCorePrerequisites.h"
#include "Components/TeCSoftBody.h"
#include "Physics/TePatchSoftBody.h"

namespace te
{
    /**
     * @copydoc	PatchSoftBody
     *
     * @note Wraps PatchSoftBody as a Component.
     */
    class TE_CORE_EXPORT CPatchSoftBody : public CSoftBody
    {
    public:
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CPatchSoftBody; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent & c, const String & suffix = "");

        /** @copydoc Component::Clone */
        bool Clone(const HPatchSoftBody& c, const String& suffix = "");

    protected:
        friend class SceneObject;

        CPatchSoftBody(); // Serialization only
        explicit CPatchSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;
    };
}
