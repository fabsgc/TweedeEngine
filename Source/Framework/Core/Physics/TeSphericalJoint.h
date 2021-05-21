#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"
#include "Math/TeVector3.h"

namespace te
{
    /**
     * A spherical joint removes all translational degrees of freedom but allows all rotational degrees of freedom.
     * Essentially this ensures that the anchor points of the two bodies are always coincident. Bodies are allowed to
     * rotate around the anchor points, and their rotatation can be limited by an elliptical cone.
     */
    class TE_CORE_EXPORT SphericalJoint : public Joint
    {
    public:
        SphericalJoint() = default;
        virtual ~SphericalJoint() = default;

        /* By default, pivot for a constraint corresponds to position of body's scene object. You can offset this position */
        virtual void SetOffsetPivot(JointBody body, const Vector3& offset);

        /** @copydoc SetOffsetPivot */
        const Vector3& GetOffsetPivot(JointBody body) const;

        /** @copydoc Joint::Update */
        virtual void Update() = 0;

        /**
         * Creates a new spherical joint.
         *
         * @param[in]	scene		Scene to which to add the joint.
         * @param[in]	desc		Settings describing the joint.
         */
        static SPtr<SphericalJoint> Create(PhysicsScene& scene);

    protected:
        Vector3 _offsetPivots[2] = { Vector3::ZERO, Vector3::ZERO };

    };
}
