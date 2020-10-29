#include "TeSceneActor.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    void SceneActor::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        _transform = transform;
        _markCoreDirty(ActorDirtyFlag::Transform);
    }

    void SceneActor::SetMobility(ObjectMobility mobility)
    {
        _mobility = mobility;
        _markCoreDirty(ActorDirtyFlag::Mobility);
    }

    void SceneActor::_updateState(const SceneObject& so, bool force)
    {
        UINT32 curHash = so.GetTransformHash();
        if (curHash != _hash || force)
        {
            SetTransform(so.GetTransform());
            _hash = curHash;
        }

        if (so.GetMobility() != _mobility || force)
            SetMobility(so.GetMobility());
    }
}
