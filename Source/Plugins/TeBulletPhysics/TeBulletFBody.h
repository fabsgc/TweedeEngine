#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFBody.h"

namespace te
{
    /** Bullet implementation of an FBody. */
    class BulletFBody : public FBody
    {
    public:
        explicit BulletFBody(btCollisionObject* body = nullptr);
        virtual ~BulletFBody() = default;

        /** Set current btCollisionObject */
        void SetBody(btCollisionObject* body) { _body = body; }

        /** Return current btCollisionObject */
        const auto& GetBody() const { return _body; }

    protected:
        friend class BulletRigidBody;
        friend class BulletMeshSoftBody;
        friend class BulletEllipsoidSoftBody;
        friend class BulletRopeSoftBody;
        friend class BulletPatchSoftBody;

    protected:
        btCollisionObject* _body;
    };
}
