#include "TeLight.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    bool Light::DefaultCastShadow = false;
    float Light::DefaultAttRadius = 1.0f;
    float Light::DefaultLinearAtt = 0.08f;
    float Light::DefaultQuadraticAtt = 0.0f;
    float Light::DefaultIntensity = 0.5f;
    float Light::DefaultSpotAngle = 45.0f;
    float Light::DefaultShadowBias = 0.5f;

    Light::Light()
        : Serializable(TID_Light)
        , _type(LightType::Directional)
        , _castShadows(DefaultCastShadow)
        , _color(Color::White)
        , _attRadius(DefaultAttRadius)
        , _linearAttenuation(DefaultLinearAtt)
        , _quadraticAttenuation(DefaultQuadraticAtt)
        , _intensity(DefaultIntensity)
        , _spotAngle(DefaultSpotAngle)
        , _shadowBias(DefaultShadowBias)
        , _rendererId(0)
    { }

    Light::Light(LightType type, Color color, float intensity, float attRadius, float linearAtt,
        float quadraticAtt, bool castShadows, Degree spotAngle)
        : Serializable(TID_Light)
        , _type(type)
        , _castShadows(castShadows)
        , _color(color)
        , _attRadius(attRadius)
        , _linearAttenuation(linearAtt)
        , _quadraticAttenuation(quadraticAtt)
        , _intensity(intensity)
        , _spotAngle(spotAngle)
        , _shadowBias(DefaultShadowBias)
        , _rendererId(0)
    { }

    Light::~Light()
    {
        if(_active)
            gRenderer()->NotifyLightRemoved(this);
    }

    void Light::Initialize()
    {
        UpdateBounds();
        gRenderer()->NotifyLightAdded(const_cast<Light*>(this));

        CoreObject::Initialize();
    }

    void Light::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Light::FrameSync()
    {
        LightType oldType = _type;
        UINT32 dirtyFlag = GetCoreDirtyFlags();
        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything | (UINT32)ActorDirtyFlag::Active;

        UpdateBounds();

        if ((dirtyFlag & updateEverythingFlag) != 0)
        {
            if (_oldActive != _active)
            {
                if (_active)
                    gRenderer()->NotifyLightAdded(this);
                else
                {
                    LightType newType = _type;
                    _type = oldType;
                    gRenderer()->NotifyLightRemoved(this);
                    _type = newType;
                }
            }
            else
            {
                LightType newType = _type;
                _type = oldType;
                gRenderer()->NotifyLightRemoved(this);
                _type = newType;
                gRenderer()->NotifyLightAdded(this);
            }
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Mobility) != 0)
        {
            // TODO I'm not sure for that, we might check if SceneActor is active
            if (_active)
            {
                gRenderer()->NotifyLightRemoved(this);
                gRenderer()->NotifyLightAdded(this);
            }
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Transform) != 0)
        {
            if (_active)
                gRenderer()->NotifyLightUpdated(this);
        }

        _oldActive = _active;
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

    void Light::SetAttenuationRadius(float radius)
    {
        _attRadius = radius;
        _markCoreDirty();
        UpdateBounds();
    }

    void Light::SetLinearAttenuation(float attenuation)
    {
        _linearAttenuation = attenuation;
        _markCoreDirty();
    }

    void Light::SetQuadraticAttenuation(float attenuation)
    {
        _quadraticAttenuation = attenuation;
        _markCoreDirty();
    }

    void Light::SetIntensity(float intensity)
    {
        _intensity = intensity;
        _markCoreDirty();
    }

    void Light::UpdateBounds()
    {
        const Transform& tfrm = GetTransform();

        switch (_type)
        {
        case LightType::Directional:
            _bounds = Sphere(tfrm.GetPosition(), std::numeric_limits<float>::infinity());
            break;
        case LightType::Radial:
            _bounds = Sphere(tfrm.GetPosition(), _attRadius * 150.0f);
            break;
        case LightType::Spot:
        {
            // Note: We could use the formula for calculating the circumcircle of a triangle (after projecting the cone),
            // but the radius of the sphere is the same as in the formula we use here, yet it is much simpler with no need
            // to calculate multiple determinants. Neither are good approximations when cone angle is wide.
            Vector3 offset(0, 0, _attRadius * 75.0f);

            // Direction along the edge of the cone, on the YZ plane (doesn't matter if we used XZ instead)
            Degree angle = Math::Clamp(_spotAngle * 0.5f, Degree(-89), Degree(89));
            Vector3 coneDir(0, Math::Tan(angle) * _attRadius * 150.0f, _attRadius * 150.0f);

            // Distance between the "corner" of the cone and our center, must be the radius (provided the center is at
            // the middle of the range)
            float radius = (offset - coneDir).Length();

            Vector3 center = tfrm.GetPosition() - tfrm.GetRotation().Rotate(offset);
            _bounds = Sphere(center, radius);
        }
        break;
        default:
            break;
        }
    }

    SPtr<Light> Light::Create(LightType type, Color color, float intensity, float attRadius, 
        float linearAtt, float quadraticAtt, bool castShadows, Degree spotAngle)
    {
        Light* handler = new (te_allocate<Light>())Light(type, color, intensity, attRadius, linearAtt, quadraticAtt, castShadows, spotAngle);
        SPtr<Light> handlerPtr = te_core_ptr<Light>(handler);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

        return handlerPtr;
    }
}
