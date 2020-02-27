#include "Components/TeCLight.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CLight::CLight()
        : Component(HSceneObject(), (UINT32)TID_CLight)
    {
        SetName("Light");
    }

    CLight::CLight(const HSceneObject& parent, LightType type, Color color,
        float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
        : Component(parent, (UINT32)TID_CLight)
        , _type(type)
        , _color(color)
        , _intensity(intensity)
        , _castsShadows(castsShadows)
        , _spotAngle(spotAngle)
    {
        SetName("Light");
    }

    CLight::~CLight()
    { 
        _internal->Destroy();
    }

    void CLight::Initialize()
    {
        Component::Initialize();
    }

    Sphere CLight::GetBounds() const
    {
        _internal->_updateState(*SO());
        return _internal->GetBounds();
    }

    void CLight::_instantiate()
    {
        _internal = Light::Create(_type, _color, _intensity, _range,
            _castsShadows, _spotAngle);
    }

    void CLight::OnCreated()
    { }

    void CLight::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, GetSceneObject());
    }

    void CLight::OnTransformChanged(TransformChangedFlags flags)
    {
        _internal->_updateState(*SO());
    }

    void CLight::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        _internal->Destroy();
    }
}
