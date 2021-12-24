#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFSoftBody.h"

namespace te
{
    /** Bullet implementation of an FSoftBody. */
    class BulletFSoftBody : public FSoftBody
    {
    public:
        BulletFSoftBody(BulletSoftBody* body, btSoftBody* btBody = nullptr);

        /** Set current btSoftBody */
        void SetBtSoftBody(btSoftBody* body) { _btSoftBody = body; }

        /** Return current btSoftBody */
        const auto& GetBtSoftBody() const { return _btSoftBody; }

        /** Set current BulletSoftBody */
        void SetSoftBody(BulletSoftBody* body) { _softBody = body; }

        /** Return current BulletSoftBody */
        const auto& GetSoftBody() const { return _softBody; }

        /** @copydoc FSoftBody::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc FSoftBody::GetScale */
        const Vector3& GetScale() const override;

    protected:
        friend class BulletMeshSoftBody;
        friend class BulletEllipsoidSoftBody;
        friend class BulletRopeSoftBody;
        friend class BulletPatchSoftBody;

    protected:
        btSoftBody* _btSoftBody;
        BulletSoftBody* _softBody;

        Vector3 _scale = Vector3::ONE;
    };
}
