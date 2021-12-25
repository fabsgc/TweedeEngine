#pragma once

#include "TeCorePrerequisites.h"
#include "Components/TeCSoftBody.h"
#include "Physics/TeEllipsoidSoftBody.h"

namespace te
{
    /**
     * @copydoc	EllipsoidSoftBody
     *
     * @note Wraps EllipsoidSoftBody as a Component.
     */
    class TE_CORE_EXPORT CEllipsoidSoftBody : public CSoftBody
    {
    public:
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CEllipsoidSoftBody; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent & c, const String & suffix = "");

        /** @copydoc Component::Clone */
        bool Clone(const HEllipsoidSoftBody& c, const String& suffix = "");

    protected:
        friend class SceneObject;

        CEllipsoidSoftBody(); // Serialization only
        explicit CEllipsoidSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;
    };
}
