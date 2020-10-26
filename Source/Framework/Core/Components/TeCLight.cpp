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
        float intensity, float range, bool castsShadows, Degree spotAngle)
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
        if(!_internal->IsDestroyed())
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
            _linearAtt, _quadraticAtt, _castsShadows, _spotAngle);
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

    void CLight::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CLight>(c));
    }

    void CLight::Clone(const HLight& c)
    {
        SPtr<Light> light = c->_getLight();

        _internal->_type = light->_type;
        _internal->_castsShadows = light->_castsShadows;
        _internal->_color = light->_color;
        _internal->_attRadius = light->_attRadius;
        _internal->_linearAttenuation = light->_linearAttenuation;
        _internal->_quadraticAttenuation = light->_quadraticAttenuation;
        _internal->_spotAngle = light->_spotAngle;
        _internal->_bounds = light->_bounds;
        _internal->_shadowBias = light->_shadowBias;

        _internal->_markCoreDirty();

        Component::Clone(c.GetInternalPtr());
    }
}
