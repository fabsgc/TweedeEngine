#include "TeBulletFSoftBody.h"
#include "TeBulletSoftBody.h"

namespace te
{ 
    BulletFSoftBody::BulletFSoftBody(BulletSoftBody* body, btSoftBody* btBody)
        : FSoftBody()
        , _softBody(body)
        , _btSoftBody(btBody)
    { }

    void BulletFSoftBody::SetScale(const Vector3& scale)
    {
        if (_scale == scale)
            return;

        _scale = scale;
        _softBody->AddToWorld();
    }

    const Vector3& BulletFSoftBody::GetScale() const
    {
        return _scale;
    }
}
