#include "Components/TeCDecal.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CDecal::CDecal()
        : Component(HSceneObject(), (UINT32)TID_CDecal)
    {
        SetName("Decal");
        SetFlag(Component::AlwaysRun, true);
    }

    CDecal::CDecal(const HSceneObject& parent)
        : Component(parent, TID_CDecal)
    {
        SetName("Decal");
        SetFlag(Component::AlwaysRun, true);
    }

    CDecal::~CDecal()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CDecal::Instantiate()
    {
        _internal = Decal::Create(HMaterial());
    }

    void CDecal::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, SO());
        Component::OnInitialized();
    }

    void CDecal::OnEnabled()
    {
        _internal->SetActive(true);
        Component::OnEnabled();
    }

    void CDecal::OnDisabled()
    {
        _internal->SetActive(false);
        Component::OnDisabled();
    }

    void CDecal::OnTransformChanged(TransformChangedFlags flags)
    {
        _internal->UpdateState(*SO());
    }

    void CDecal::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        Component::OnDestroyed();
        _internal->Destroy();
    }

    bool CDecal::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CDecal>(c), suffix);
    }

    bool CDecal::Clone(const HDecal& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            SPtr<Decal> decal = c->GetInternal();
            if (decal)
            {
                _internal->_markCoreDirty();

                _internal->_bounds = decal->_bounds;
                _internal->_material = decal->_material;
                _internal->_tfrmMatrix = decal->_tfrmMatrix;
                _internal->_tfrmMatrixNoScale = decal->_tfrmMatrixNoScale;
                _internal->_maxDistance = decal->_maxDistance;
                _internal->_size = decal->_size;
                _internal->_layer = decal->_layer;
                _internal->_layerMask = decal->_layerMask;

                _internal->_transform = decal->_transform;
                _internal->_mobility = decal->_mobility;

                _internal->_markCoreDirty();
            }

            return true;
        }

        return false;
    }
}
