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

        /** @copydoc ConeTwistJoint::SetSoftnessLimit */
        void SetSoftnessLimit(float softness);

        /** @copydoc ConeTwistJoint::GetSoftnessLimit */
        const float GetSoftnessLimit() const { return _softnessLimit; }

        /** @copydoc ConeTwistJoint::SetBiasLimit */
        void SetBiasLimit(float bias);

        /** @copydoc ConeTwistJoint::GetBiasLimit */
        const float GetBiasLimit() const { return _biasLimit; }

        /** @copydoc ConeTwistJoint::SetRelaxationLimit */
        void SetRelaxationLimit(float relaxation);

        /** @copydoc ConeTwistJoint::GetRelaxationLimit */
        const float GetRelaxationLimit() const { return _relaxationLimit; }

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

        /** @copydoc ConeTwistJoint::SetMotorEnabled */
        void SetMotorEnabled(bool motorEnabled);

        /** @copydoc ConeTwistJoint::GetMotorEnabled */
        const bool GetMotorEnabled() const { return _motorEnabled; }

        /** @copydoc ConeTwistJoint::SetMaxMotorImpulse */
        void SetMaxMotorImpulse(float motorImpulse);

        /** @copydoc ConeTwistJoint::GetMaxMotorImpulse */
        const bool GetMaxMotorImpulse() const { return _motorImpulse; }

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the Cone Twist joint that this component wraps. */
        ConeTwistJoint* GetInternal() const { return static_cast<ConeTwistJoint*>(_internal.get()); }

    protected:
        CConeTwistJoint(); // Serialization only

    protected:
        float _damping = 0.0f;
        float _softnessLimit = 1.0f;
        float _biasLimit = 0.3f;
        float _relaxationLimit = 1.0f;
        Degree _swingSpan1;
        Degree _swingSpan2;
        Degree _twistSpan;
        bool _angularOnly = false;
        bool _motorEnabled = false;
        float _motorImpulse = 0.0f;
    };
}
