#include "TeBulletFBody.h"

namespace te
{ 
    BulletFBody::BulletFBody(btCollisionObject* body)
        : FBody(CoreType::TID_FBody)
        , _body(body)
    { }
}
