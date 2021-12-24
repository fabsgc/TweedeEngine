#include "TeBulletFSoftBody.h"

namespace te
{ 
    BulletFSoftBody::BulletFSoftBody(btCollisionObject* body)
        : FSoftBody()
        , _body(body)
    { }
}
