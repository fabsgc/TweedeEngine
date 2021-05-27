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
    };
}
