#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    /**
     * Joint that removes all but a single translational degree of freedom. Bodies are allowed to move along a single axis.
     */
    class TE_CORE_EXPORT SliderJoint : public Joint
    {
    public:
        SliderJoint();
        virtual ~SliderJoint() = default;

        /** @copydoc Joint::Update */
        virtual void Update() = 0;

        /** */
        virtual void SetLowerLinLimit(float lowerLinLimit);

        /** @copydoc SetLowerLinLimit */
        const float GetLowerLinLimit() const { return _lowerLinLimit; }

        /** */
        virtual void SetUpperLinLimit(float upperLinLimit);

        /** @copydoc SetUpperLinLimit */
        const float GetUpperLinLimit() const { return _upperLinLimit; }

        /** */
        virtual void SetLowerAngLimit(Degree lowerAngLimit);

        /** @copydoc SetLowerAngLimit */
        const Degree GetLowerAngLimit() const { return _lowerAngLimit; }

        /** */
        virtual void SetUpperAngLimit(Degree upperAngLimit);

        /** @copydoc SetUpperAngLimit */
        const Degree GetUpperAngLimit() const { return _upperAngLimit; }

        /** */
        virtual void SetSoftnessDirLin(float softnessDirLin);

        /** @copydoc SetSoftnessDirLin */
        virtual float GetSoftnessDirLin() const { return _softnessDirLin; }

        /** */
        virtual void SetRestitutionDirLin(float restitutionDirLin);

        /** @copydoc SetRestitutionDirLin */
        virtual float GetRestitutionDirLin() const { return _restitutionDirLin; }

        /** */
        virtual void SetDampingDirLin(float dampingDirLin);

        /** @copydoc Set */
        virtual float GetDampingDirLin() const { return _dampingDirLin; }

        /** */
        virtual void SetSoftnessDirAng(float softnessDirAng);

        /** @copydoc SetSoftnessDirAng */
        virtual float GetSoftnessDirAng() const { return _softnessDirAng; }

        /** */
        virtual void SetRestitutionDirAng(float restitutionDirAng);

        /** @copydoc SetRestitutionDirAng */
        virtual float GetRestitutionDirAng() const { return _restitutionDirAng; }

        /** */
        virtual void SetDampingDirAng(float dampingDirAng);

        /** @copydoc SetDampingDirAng */
        virtual float GetDampingDirAng() const { return _dampingDirAng; }

        /** */
        virtual void SetSoftnessLimLin(float softnessLimLin);

        /** @copydoc Set */
        virtual float GetSoftnessLimLin() const { return _softnessLimLin; }

        /** */
        virtual void SetRestitutionLimLin(float restitutionLimLin);

        /** @copydoc SetRestitutionLimLin */
        virtual float GetRestitutionLimLin() const { return _restitutionLimLin; }

        /** */
        virtual void SetDampingLimLin(float dampingLimLin);

        /** @copydoc SetDampingLimLin */
        virtual float GetDampingLimLin() const { return _dampingLimLin; }

        /**
         * Creates a new slider joint.
         *
         * @param[in]	scene		Scene to which to add the joint.
         */
        static SPtr<SliderJoint> Create(PhysicsScene& scene);

    protected:
        float _lowerLinLimit = 1.0f;
        float _upperLinLimit = -1.0f;
        Degree _lowerAngLimit;
        Degree _upperAngLimit;

        float _softnessDirLin = 1.0f;
        float _restitutionDirLin = 0.7f;
        float _dampingDirLin = 0.0f;
        float _softnessDirAng = 1.0f;
        float _restitutionDirAng = 0.7f;
        float _dampingDirAng = 0.0f;
        float _softnessLimLin = 1.0f;
        float _restitutionLimLin = 0.7f;
        float _dampingLimLin = 1.0f;
    };
}
