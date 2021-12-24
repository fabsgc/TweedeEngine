#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFSoftBody.h"

namespace te
{
    /** Bullet implementation of an FSoftBody. */
    class BulletFSoftBody : public FSoftBody
    {
    public:
        explicit BulletFSoftBody(btCollisionObject* body = nullptr);
        ~BulletFSoftBody() = default;

        /** Set current btCollisionObject */
        void SetBody(btCollisionObject* body) { _body = body; }

        /** Return current btCollisionObject */
        const auto& GetBody() const { return _body; }

    protected:
        friend class BulletRigidBody;
        friend class BulletSoftBody;
        friend class BulletMeshSoftBody;
        friend class BulletEllipsoidSoftBody;
        friend class BulletRopeSoftBody;
        friend class BulletPatchSoftBody;

    protected:

        btCollisionObject* _body;
    };
}
