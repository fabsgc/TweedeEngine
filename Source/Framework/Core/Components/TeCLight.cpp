#include "Components/TeCLight.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    CLight::CLight()
        : Component(HSceneObject(), CoreType::TID_CLight)
    {
        SetName("Light");
        SetFlag(Component::AlwaysRun, true);
    }

    CLight::CLight(const HSceneObject& parent, Light::Type type, Color color,
        float intensity, bool castShadows, Light::CastShadowsType castShadowsType, Degree spotAngle)
        : Component(parent, CoreType::TID_CLight)
        , _type(type)
        , _color(color)
        , _intensity(intensity)
        , _castShadows(castShadows)
        , _spotAngle(spotAngle)
        , _castShadowsType(castShadowsType)
    {
        SetName("Light");
        SetFlag(Component::AlwaysRun, true);
    }

    CLight::~CLight()
    { 
        if(_internal && !_internal->IsDestroyed())
            _internal->Destroy();
    }

    Sphere CLight::GetBounds() const
    {
        _internal->UpdateState(*SO());
        return _internal->GetBounds();
    }

    void CLight::Instantiate()
    {
        _internal = Light::Create(_type, _color, _intensity, _castShadows, _castShadowsType, _spotAngle);
        _internal->AttachTo(gRenderer());
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

        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
        _internal = nullptr;
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
            SPtr<Light> light = c->GetInternal();
            if (light)
            {
                _internal->_type = light->_type;
                _internal->_castShadows = light->_castShadows;
                _internal->_color = light->_color;
                _internal->_spotAngle = light->_spotAngle;
                _internal->_bounds = light->_bounds;
                _internal->_shadowBias = light->_shadowBias;
                _internal->_intensity = light->_intensity;
                _internal->_castShadowsType = light->_castShadowsType;
                _internal->_layer = light->_layer;

                _internal->_transform = light->_transform;
                _internal->_mobility = light->_mobility;

                c->_type = _type;
                c->_color = _color;
                c->_intensity = _intensity;
                c->_castShadows = _castShadows;
                c->_spotAngle = _spotAngle;
                c->_castShadowsType = _castShadowsType;

                _internal->_markCoreDirty();
            }

            return true;
        }

        return false;
    }

    void CLight::ExportJson(nlohmann::json& document) const
    {
        Component::ExportJson(document);

        document["type"] = GetComponentType();

        if (_internal)
        {
            auto& lightDoc = document["light"];
            lightDoc["type"] = static_cast<uint32_t>(_internal->GetType());
            lightDoc["castShadows"] = _internal->GetCastShadows();
            lightDoc["castShadowsType"] = static_cast<uint32_t>(_internal->GetCastShadowsType());
            lightDoc["spotAngle"] = _internal->GetSpotAngle().ValueRadians();
            lightDoc["shadowBias"] = _internal->GetShadowBias();
            lightDoc["intensity"] = _internal->GetIntensity();
            lightDoc["layer"] = _internal->GetLayer();
            
            _internal->GetColor().ExportJson(lightDoc["color"]);
        }
    }

    bool CLight::ImportJson(const nlohmann::json& document, CLight& object)
    {
        Component::ImportJson(document, object);

        if (document.contains("light"))
        {
            const auto& lightDoc = document["light"];

            if (lightDoc.contains("type"))
                object.SetType(static_cast<Light::Type>(lightDoc["type"].get<uint32_t>()));

            if (lightDoc.contains("castShadows"))
                object.SetCastShadows(lightDoc["castShadows"].get<bool>());

            if (lightDoc.contains("spotAngle"))
                object.SetSpotAngle(Degree(lightDoc["spotAngle"].get<float>()));

            if (lightDoc.contains("shadowBias"))
                object.SetShadowBias(lightDoc["shadowBias"].get<float>());

            if (lightDoc.contains("intensity"))
                object.SetIntensity(lightDoc["intensity"].get<float>());

            if (lightDoc.contains("castShadowType"))
                object.SetCastShadowsType(static_cast<Light::CastShadowsType>(lightDoc["castShadowType"].get<uint32_t>()));

            if (lightDoc.contains("layer"))
                object.SetLayer(lightDoc["layer"].get<UINT32>());

            if (lightDoc.contains("color"))
                object.SetColor(Color::ImportJson(lightDoc["color"]));
        }
        else
        {
            TE_DEBUG("Failed to import CLight from JSON, missing 'light' section.");
            return false;
        }

        return true;
    }
}
