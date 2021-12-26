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

        /** @copydoc PatchSoftBody::SetCorners */
        void SetCorners(const Vector3& topLeft, const Vector3& topRight, const Vector3& bottomLeft, const Vector3& bottomRight);

        /** @copydoc PatchSoftBody::SetCorners */
        void GetCorners(Vector3& topLeft, Vector3& topRight, Vector3& bottomLeft, Vector3& bottomRight);

        /** @copydoc PatchSoftBody::SetCorners */
        void SetResolution(UINT32 x, UINT32 y);

        /** @copydoc PatchSoftBody::SetCorners */
        void GetResolution(UINT32& x, UINT32& y);

    protected:
        friend class SceneObject;

        CPatchSoftBody(); // Serialization only
        explicit CPatchSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

    protected:
        Vector3 _topLeft;
        Vector3 _topRight;
        Vector3 _bottomLeft;
        Vector3 _bottomRight;
        UINT32 _resolutionX;
        UINT32 _resolutionY;
    };
}
