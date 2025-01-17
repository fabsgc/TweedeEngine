#include "TeBulletFBody.h"

namespace te
{ 
    BulletFBody::BulletFBody(btCollisionObject* body)
        : FBody(TID_FBody)
        , _body(body)
    { }
}
