#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeD6Joint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	D6Joint
     *
     * @note	Wraps D6Joint as a Component.
     */
    class TE_CORE_EXPORT CD6Joint : public CJoint
    {
    public:
        CD6Joint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CD6Joint; }

        /** @copydoc Component::Clone */
        void Clone(const HD6Joint& c);

        /** @copydoc D6Joint::SetLowerLinLimit */
        void SetLowerLinLimit(float lowerLinLimit);

        /** @copydoc SetLowerLinLimit */
        float GetLowerLinLimit() const { return _lowerLinLimit; }

        /** @copydoc D6Joint::SetUpperLinLimit */
        void SetUpperLinLimit(float upperLinLimit);

        /** @copydoc D6Joint::GetUpperLinLimit */
        float GetUpperLinLimit() const { return _upperLinLimit; }

        /** @copydoc D6Joint::SetLowerAngLimit */
        void SetLowerAngLimit(Degree lowerAngLimit);

        /** @copydoc D6Joint::GetLowerAngLimit */
        const Degree GetLowerAngLimit() const { return _lowerAngLimit; }

        /** @copydoc D6Joint::SetUpperAngLimit */
        void SetUpperAngLimit(Degree upperAngLimit);

        /** @copydoc D6Joint::GetUpperAngLimit */
        const Degree GetUpperAngLimit() const { return _upperAngLimit; }

        /** @copydoc D6Joint::SetLinearSpring */
        void SetLinearSpring(bool linearSpring);

        /** @copydoc D6Joint::GetLinearSpring */
        bool GetLinearSpring() const { return _linearSpring; }

        /** @copydoc D6Joint::SetAngularSpring */
        void SetAngularSpring(bool angularSpring);

        /** @copydoc D6Joint::GetLinearSpring */
        bool GetAngularSpring() const { return _angularSpring; }

        /** @copydoc D6Joint::SetLinearStiffness */
        void SetLinearStiffness(float linearStiffness);

        /** @copydoc D6Joint::GetLinearStiffness */
        float GetLinearStiffness() const { return _linearStiffness; }

        /** @copydoc D6Joint::SetAngularStiffness */
        void SetAngularStiffness(float angularStiffness);

        /** @copydoc D6Joint::GetAngularStiffness */
        float GetAngularStiffness() const { return _angularStiffness; }

        /** @copydoc D6Joint::SetLinearDamping */
        void SetLinearDamping(float linearDamping);

        /** @copydoc D6Joint::GetLinearDamping */
        float GetLinearDamping() const { return _linearDamping; }

        /** @copydoc D6Joint::SetAngularDamping */
        void SetAngularDamping(float angularDamping);

        /** @copydoc D6Joint::GetAngularDamping */
        float GetAngularDamping() const { return _angularDamping; }

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the 6Dof joint that this component wraps. */
        D6Joint* GetInternal() const { return static_cast<D6Joint*>(_internal.get()); }

    protected:
        CD6Joint(); // Serialization only

    protected:
        float _lowerLinLimit = 0.0f;
        float _upperLinLimit = 0.0f;
        Degree _lowerAngLimit;
        Degree _upperAngLimit;

        bool _linearSpring = false;
        bool _angularSpring = false;
        float _linearStiffness = 0.0f;
        float _angularStiffness = 0.0f;
        float _linearDamping = 1.0f;
        float _angularDamping = 1.0f;
    };
}
