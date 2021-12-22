#include "Components/TeCLight.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    CLight::CLight()
        : Component(HSceneObject(), (UINT32)TID_CLight)
    {
        SetName("Light");
        SetFlag(Component::AlwaysRun, true);
    }

    CLight::CLight(const HSceneObject& parent, LightType type, Color color,
        float intensity, float range, bool castShadows, Degree spotAngle)
        : Component(parent, (UINT32)TID_CLight)
        , _type(type)
        , _color(color)
        , _intensity(intensity)
        , _castShadows(castShadows)
        , _spotAngle(spotAngle)
    {
        SetName("Light");
        SetFlag(Component::AlwaysRun, true);
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
        _internal->UpdateState(*SO());
        return _internal->GetBounds();
    }

    void CLight::Instantiate()
    {
        _internal = Light::Create(_type, _color, _intensity, _range, 
            _linearAtt, _quadraticAtt, _castShadows, _spotAngle);
    }

    void CLight::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, GetSceneObject());
        Component::OnInitialized();
    }

    void CLight::OnEnabled()
    {
        _internal->SetActive(true);
        Component::OnEnabled();
    }

    void CLight::OnDisabled()
    {
        _internal->SetActive(false);
        Component::OnDisabled();
    }

    void CLight::OnTransformChanged(TransformChangedFlags flags)
    {
        _internal->UpdateState(*SO());
    }

    void CLight::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        Component::OnDestroyed();
        _internal->Destroy();
    }

    bool CLight::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CLight>(c), suffix);
    }

    bool CLight::Clone(const HLight& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            SPtr<Light> light = c->_getLight();
            if (light)
            {
                _internal->_type = light->_type;
                _internal->_castShadows = light->_castShadows;
                _internal->_color = light->_color;
                _internal->_attRadius = light->_attRadius;
                _internal->_linearAttenuation = light->_linearAttenuation;
                _internal->_quadraticAttenuation = light->_quadraticAttenuation;
                _internal->_spotAngle = light->_spotAngle;
                _internal->_bounds = light->_bounds;
                _internal->_shadowBias = light->_shadowBias;

                _internal->_transform = light->_transform;
                _internal->_mobility = light->_mobility;

                c->_type = _type;
                c->_color = _color;
                c->_intensity = _intensity;
                c->_range = _range;
                c->_linearAtt = _linearAtt;
                c->_quadraticAtt = _quadraticAtt;
                c->_castShadows = _castShadows;
                c->_spotAngle = _spotAngle;

                _internal->_markCoreDirty();
            }

            return true;
        }

        return false;
    }
}
