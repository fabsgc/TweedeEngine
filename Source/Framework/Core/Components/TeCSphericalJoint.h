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
        static UINT32 GetComponentType() { return TID_CSphericalJoint; }

        /** @copydoc Component::Clone */
        void Clone(const HSphericalJoint& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        SPHERICAL_JOINT_DESC _desc;

    protected:
        CSphericalJoint(); // Serialization only
    };
}
