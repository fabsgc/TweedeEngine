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
        ~BulletFBody() = default;

        /** Set current btCollisionObject */
        void SetBody(btCollisionObject* body) { _body = body; }

        /** Return current btCollisionObject */
        const auto& GetBody() const { return _body; }

    protected:
        friend class BulletRigidBody;
        friend class BulletSoftBody;

        btCollisionObject* _body;
    };
}
