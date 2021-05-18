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

        /** @copydoc SphericalJoint::SetPivot */
        void SetPivot(JointBody body, const Vector3& pivot);

        /** @copydoc SphericalJoint::GetPivot */
        Vector3 GetPivot(JointBody body);

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

    protected:
        Vector3 _pivots[2] = { Vector3::ZERO, Vector3::ZERO };
    };
}
