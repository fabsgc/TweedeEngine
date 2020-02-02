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
    }

    void Light::FrameSync()
    {
        TE_PRINT("# SYNC LIGHT");

        LightType oldType = _type;
        UINT32 dirtyFlag = GetCoreDirtyFlags();
        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything;

        UpdateBounds();

        if ((dirtyFlag & updateEverythingFlag) != 0)
        {
            LightType newType = _type;
            _type = oldType;
            gRenderer()->NotifyLightRemoved(this);
            _type = newType;
            gRenderer()->NotifyLightAdded(this);
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Mobility) != 0)
        {
            gRenderer()->NotifyLightRemoved(this);
            gRenderer()->NotifyLightAdded(this);
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Transform) != 0)
        {
            gRenderer()->NotifyLightUpdated(this);
        }
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
