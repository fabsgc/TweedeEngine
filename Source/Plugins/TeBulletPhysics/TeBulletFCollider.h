#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeFCollider.h"

namespace te
{
    /** Bullet implementation of FCollider. */
    class BulletFCollider : public FCollider
    {
    public:
        explicit BulletFCollider(BulletPhysics* physics, BulletScene* scene, btCollisionShape* shape = nullptr);

        /** @copydoc FCollider::SetCenter */
        void SetCenter(const Vector3& center) override;

        /** @copydoc FCollider::SetPosition */
        void SetPosition(const Vector3& position) override;

        /** @copydoc FCollider::SetRotation */
        void SetRotation(const Quaternion& rotation) override;

        /** Set current btCollisionShape */
        void SetShape(btCollisionShape* shape) { _shape = shape; }

        /** Return current btCollisionShape */
        const auto& GetShape() const { return _shape; }

        /** Using relative position and rotation of collider, build associated btTransform */
        const btTransform& GetBtTransform();

    protected:
        BulletFCollider(); // Serialization only

        /** @copydoc GetBtTransform */
        void SetBtTransform();

    protected:
        BulletPhysics* _physics;
        BulletScene* _scene;

        btCollisionShape* _shape;
        btTransform _tranform;
    };
}
