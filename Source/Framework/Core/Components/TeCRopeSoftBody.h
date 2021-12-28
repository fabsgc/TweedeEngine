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

        /** @copydoc RopeSoftBody::SetFrom */
        void SetFrom(const Vector3& from);

        /** @copydoc RopeSoftBody::GetFrom */
        Vector3 GetFrom() const { return _from; }

        /** @copydoc RopeSoftBody::SetTo */
        void SetTo(const Vector3& to);

        /** @copydoc RopeSoftBody::GetTo */
        Vector3 GetTo() const { return _to; }

        /** @copydoc RopeSoftBody::SetResolution */
        void SetResolution(UINT32 resolution);

        /** @copydoc RopeSoftBody::GetResolution */
        UINT32 GetResolution() const { return _resolution; }

    protected:
        friend class SceneObject;

        CRopeSoftBody(); // Serialization only
        explicit CRopeSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

    protected:
        Vector3 _from = Vector3(-2.0f, 10.0f, -2.0f);
        Vector3 _to = Vector3(0.0f, 2.0f, 0.0f);
        UINT32 _resolution = 16;
    };
}
