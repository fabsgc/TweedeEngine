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

        /** @copydoc ConeTwistJoint::SetDamping */
        void SetDamping(float damping);

        /** @copydoc ConeTwistJoint::GetDamping */
        const float GetDamping() const { return _damping; }

        /** @copydoc ConeTwistJoint::SetLimitSoftness */
        void SetLimitSoftness(float softness);

        /** @copydoc ConeTwistJoint::GetLimitSoftness */
        const float GetLimitSoftness() const { return _limitSoftness; }

        /** @copydoc ConeTwistJoint::SetLimitBias */
        void SetLimitBias(float bias);

        /** @copydoc ConeTwistJoint::GetLimitBias */
        const float GetLimitBias() const { return _limitBias; }

        /** @copydoc ConeTwistJoint::SetLimitRelaxation */
        void SetLimitRelaxation(float relaxation);

        /** @copydoc ConeTwistJoint::GetLimitRelaxation */
        const float GetLimitRelaxation() const { return _limitRelaxation; }

        /** @copydoc ConeTwistJoint::SetSwingSpan1 */
        void SetSwingSpan1(Degree deg);

        /** @copydoc ConeTwistJoint::GetSwingSpan1 */
        const Degree GetSwingSpan1() const { return _swingSpan1; }

        /** @copydoc ConeTwistJoint::SetSwingSpan2 */
        void SetSwingSpan2(Degree deg);

        /** @copydoc ConeTwistJoint::GetSwingSpan2 */
        const Degree GetSwingSpan2() const { return _swingSpan2; }

        /** @copydoc ConeTwistJoint::SetTwistSpan */
        void SetTwistSpan(Degree deg);

        /** @copydoc ConeTwistJoint::GetTwistSpan */
        const Degree GetTwistSpan() const { return _twistSpan; }

        /** ConeTwistJoint::SetAngularOnly */
        void SetAngularOnly(bool angularOnly);

        /** @copydoc ConeTwistJoint::GetAngularOnly */
        const bool GetAngularOnly() const { return _angularOnly; }

    protected:
        friend class SceneObject;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

    protected:
        CConeTwistJoint(); // Serialization only

    protected:
        float _damping = 0.0f;
        float _limitSoftness = 1.0f;
        float _limitBias = 0.3f;
        float _limitRelaxation = 1.0f;
        Degree _swingSpan1;
        Degree _swingSpan2;
        Degree _twistSpan;
        bool _angularOnly = false;
    };
}
