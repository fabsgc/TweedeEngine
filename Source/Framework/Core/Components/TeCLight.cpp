#include "Components/TeCLight.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CLight::CLight()
        : Component(HSceneObject(), (UINT32)TID_CLight)
    {
        SetName("Light");
    }

    CLight::CLight(const HSceneObject& parent, LightType type)
        : Component(parent, (UINT32)TID_CLight)
        , _type(type)
    {
        SetName("Light");
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
    { }

    void CLight::OnCreated()
    { }

    void CLight::OnInitialized()
    {
        _internal = Light::Create(_type);
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

    CLight::~CLight()
    { }
}
