#include "TeLight.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    Light::Light()
        : _type(LightType::Directional)
    { }

    Light::Light(LightType type)
        : _type(type)
    { }

    Light::~Light()
    {
        gRenderer()->NotifyLightRemoved(this);
    }

    void Light::Initialize()
    { 
        CoreObject::Initialize();
        gRenderer()->NotifyLightAdded(const_cast<Light*>(this));
    }

    void Light::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
        gRenderer()->NotifyLightUpdated(const_cast<Light*>(this));
    }

    void Light::SetMobility(ObjectMobility mobility)
    {
        SceneActor::SetMobility(mobility);
        _markCoreDirty(ActorDirtyFlag::Mobility);
    }

    void Light::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        SceneActor::SetTransform(transform);
        UpdateBounds();
    }

    void Light::UpdateBounds()
    {
        const Transform& tfrm = GetTransform();
        _bounds = Sphere(tfrm.GetPosition(), std::numeric_limits<float>::infinity());
    }

    SPtr<Light> Light::Create(LightType type)
    {
        Light* handler = new (te_allocate<Light>())Light(type);
        SPtr<Light> handlerPtr = te_core_ptr<Light>(handler);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

        return handlerPtr;
    }
}
