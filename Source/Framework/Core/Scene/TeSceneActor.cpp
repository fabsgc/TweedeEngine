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

    void SceneActor::SetActive(bool active)
    {
        _active = active;
        _markCoreDirty(ActorDirtyFlag::Active);
    }

    void SceneActor::_updateState(const SceneObject& so, bool force)
    {
        UINT32 curHash = so.GetTransformHash();
        if (curHash != _hash || force)
        {
            SetTransform(so.GetTransform());
            _hash = curHash;
        }

        if (so.GetActive() != _active || force)
        {
            SetActive(so.GetActive());
            _markCoreDirty(ActorDirtyFlag::Active);
        }

        if (so.GetMobility() != _mobility || force)
        {
            SetMobility(so.GetMobility());
            _markCoreDirty(ActorDirtyFlag::Mobility);
        }
    }
}
