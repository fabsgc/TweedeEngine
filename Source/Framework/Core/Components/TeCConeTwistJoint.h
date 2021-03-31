#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeConeTwistJoint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	ConeTwistJoint
     *
     * @note	Wraps ConeTwistJoint as a Component.
     */
    class TE_CORE_EXPORT CConeTwistJoint : public CJoint
    {
    public:
        CConeTwistJoint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CConeTwistJoint; }

        /** @copydoc Component::Clone */
        void Clone(const HConeTwistJoint& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        CONE_TWIST_JOINT_DESC _desc;

    protected:
        CConeTwistJoint(); // Serialization only
    };
}
