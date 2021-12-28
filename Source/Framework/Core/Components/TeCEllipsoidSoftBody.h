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

        /** @copydoc EllipsoidSoftBody::SetCenter */
        void SetCenter(const Vector3& center);

        /** @copydoc EllipsoidSoftBody::GetCenter */
        Vector3 GetCenter() const { return _center; }

        /** @copydoc EllipsoidSoftBody::SetRadius */
        void SetRadius(const Vector3& radius);

        /** @copydoc EllipsoidSoftBody::GetRadius */
        Vector3 GetRadius() const { return _radius; }

        /** @copydoc EllipsoidSoftBody::SetResolution */
        void SetResolution(UINT32 resolution);

        /** @copydoc EllipsoidSoftBody::GetResolution */
        UINT32 GetResolution() const { return _resolution; }

    protected:
        friend class SceneObject;

        CEllipsoidSoftBody(); // Serialization only
        explicit CEllipsoidSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

    protected:
        Vector3 _center = Vector3::ZERO;
        Vector3 _radius = Vector3::ONE;
        UINT32 _resolution = 16;
    };
}
