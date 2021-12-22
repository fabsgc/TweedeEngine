#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSphericalJoint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	SphericalJoint
     *
     * @note	Wraps SphericalJoint as a Component.
     */
    class TE_CORE_EXPORT CSphericalJoint : public CJoint
    {
    public:
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CSphericalJoint; }

        /** @copydoc Component::Clone */
        bool Clone(const HSphericalJoint& c, const String& suffix = "");

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

    protected:
        friend class SceneObject;

        CSphericalJoint(); // Serialization only
        CSphericalJoint(const HSceneObject& parent);

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the spherical joint that this component wraps. */
        SphericalJoint* GetInternal() const { return static_cast<SphericalJoint*>(_internal.get()); }
    };
}
