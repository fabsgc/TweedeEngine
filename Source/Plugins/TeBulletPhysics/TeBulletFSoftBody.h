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

    protected:

        btCollisionObject* _body;
    };
}
