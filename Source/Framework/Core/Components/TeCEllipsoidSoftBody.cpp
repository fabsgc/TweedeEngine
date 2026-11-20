#include "Components/TeCEllipsoidSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CEllipsoidSoftBody::CEllipsoidSoftBody()
        : CSoftBody(HSceneObject(), CoreType::TID_CEllipsoidSoftBody)
    {
        SetName("EllipsoidSoftBody");
    }

    CEllipsoidSoftBody::CEllipsoidSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, CoreType::TID_CEllipsoidSoftBody)
    {
        SetName("EllipsoidSoftBody");
    }

    bool CEllipsoidSoftBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CEllipsoidSoftBody>(c), suffix);
    }

    bool CEllipsoidSoftBody::Clone(const HEllipsoidSoftBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CSoftBody::Clone(static_object_cast<CSoftBody>(c), suffix))
        {
            _center = c->_center;
            _radius = c->_radius;
            _resolution = c->_resolution;

            return true;
        }

        return false;
    }

    void CEllipsoidSoftBody::SetCenter(const Vector3& center)
    {
        if (_center == center)
            return;

        _center = center;

        if (_internal != nullptr)
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetCenter(center);
    }

    void CEllipsoidSoftBody::SetRadius(const Vector3& radius)
    {
        if (_radius == radius)
            return;

        _radius = radius;

        if (_internal != nullptr)
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetRadius(radius);
    }

    void CEllipsoidSoftBody::SetResolution(UINT32 resolution)
    {
        if (_resolution == resolution)
            return;

        _resolution = resolution;

        if (_internal != nullptr)
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetResolution(resolution);
    }

    void CEllipsoidSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
        { 
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetCenter(_center);
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetRadius(_radius);
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetResolution(_resolution);
        }
    }

    SPtr<Body> CEllipsoidSoftBody::CreateInternal()
    {
        SPtr<EllipsoidSoftBody> body = EllipsoidSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }

    void CEllipsoidSoftBody::ExportJson(nlohmann::json& document) const
    {
        CSoftBody::ExportJson(document);

        document["type"] = GetComponentType();

        _center.ExportJson(document["center"]);
        _radius.ExportJson(document["radius"]);
        document["resolution"] = _resolution;
    }

    bool CEllipsoidSoftBody::ImportJson(const nlohmann::json& document, CEllipsoidSoftBody& softBody)
    {
        CSoftBody::ImportJson(document, softBody);

        if (document.contains("center"))
            softBody.SetCenter(Vector3::ImportJson(document["center"]));
        if (document.contains("radius"))
            softBody.SetRadius(Vector3::ImportJson(document["radius"]));
        if (document.contains("resolution"))
            softBody.SetResolution(document["resolution"].get<UINT32>());

        return true;
    }
}
