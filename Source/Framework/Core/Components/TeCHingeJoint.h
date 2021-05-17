#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeHingeJoint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	HingeJoint
     *
     * @note	Wraps HingeJoint as a Component.
     */
    class TE_CORE_EXPORT CHingeJoint : public CJoint
    {
    public:
        CHingeJoint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CHingeJoint; }

        /** @copydoc Component::Clone */
        void Clone(const HHingeJoint& c);

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

    protected:
        CHingeJoint(); // Serialization only
    };
}
