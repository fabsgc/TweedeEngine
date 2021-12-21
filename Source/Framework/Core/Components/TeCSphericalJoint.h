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
        CSphericalJoint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CSphericalJoint; }

        /** @copydoc Component::Clone */
        bool Clone(const HSphericalJoint& c, const String& suffix = "");

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the spherical joint that this component wraps. */
        SphericalJoint* GetInternal() const { return static_cast<SphericalJoint*>(_internal.get()); }

    protected:
        CSphericalJoint(); // Serialization only
    };
}
