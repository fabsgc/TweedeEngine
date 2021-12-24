#include "TeBulletFBody.h"

namespace te
{ 
    BulletFBody::BulletFBody(btCollisionObject* body)
        : FBody()
        , _body(body)
    { }
}
