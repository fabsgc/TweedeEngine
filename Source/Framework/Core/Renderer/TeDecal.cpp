#include "TeDecal.h"
#include "Renderer/TeRenderer.h"
#include "Material/TeMaterial.h"

namespace te
{
    Decal::Decal(const HMaterial& material, const Vector2& size, float maxDistance)
        : Serializable(TID_Decal)
        , _size(size)
        , _maxDistance(maxDistance)
    {
        if (material.IsLoaded())
            _material = material.GetInternalPtr();

        UpdateBounds();
    }

    Decal::Decal(const SPtr<Material>& material, const Vector2& size, float maxDistance)
        : Serializable(TID_Decal)
        , _material(material)
        , _size(size)
        , _maxDistance(maxDistance)
    {
        UpdateBounds();
    }

    Decal::Decal()
        : Serializable(TID_Decal)
    { }

    Decal::~Decal()
    { 
        if (_active)
            gRenderer()->NotifyDecalRemoved(this);
    }

    void Decal::Initialize()
    { 
        UpdateBounds();
        gRenderer()->NotifyDecalAdded(this);

        CoreObject::Initialize();
    }

    void Decal::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Decal::FrameSync()
    { 
        _tfrmMatrix = _transform.GetMatrix();
        _tfrmMatrixNoScale = Matrix4::TRS(_transform.GetPosition(), _transform.GetRotation(), Vector3::ONE);

        UpdateBounds();

        UINT32 dirtyFlag = GetCoreDirtyFlags();

        if (dirtyFlag == (UINT32)ActorDirtyFlag::Transform)
        {
            if (_active)
                gRenderer()->NotifyDecalUpdated(this);
        }
        else
        {
            if (_oldActive != GetActive())
            {
                if (_active)
                    gRenderer()->NotifyDecalAdded(this);
                else
                    gRenderer()->NotifyDecalRemoved(this);
            }
            else
            {
                gRenderer()->NotifyDecalRemoved(this);
                gRenderer()->NotifyDecalAdded(this);
            }
        }

        _oldActive = _active;
    }

    void Decal::SetLayer(UINT64 layer)
    {
        const bool isPow2 = layer && !((layer - 1) & layer);

        if (!isPow2)
        {
            TE_DEBUG("Invalid layer provided. Only one layer bit may be set. Ignoring.");
            return;
        }

        _layer = layer;
        _markCoreDirty();
    }

    void Decal::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        _transform = transform;
        _tfrmMatrix = transform.GetMatrix();
        _tfrmMatrixNoScale = Matrix4::TRS(transform.GetPosition(), transform.GetRotation(), Vector3::ONE);

        _markCoreDirty(ActorDirtyFlag::Transform);
    }

    void Decal::UpdateBounds()
    {
        const Vector2& extents = _size * 0.5f;

        AABox localAABB(
            Vector3(-extents.x, -extents.y, -_maxDistance),
            Vector3(extents.x, extents.y, 0.0f)
        );

        localAABB.TransformAffine(_tfrmMatrix);

        _bounds = Bounds(localAABB, Sphere(localAABB.GetCenter(), localAABB.GetRadius()));
    }

    SPtr<Decal> Decal::Create(const HMaterial& material, const Vector2& size, float maxDistance)
    {
        Decal* handler = new (te_allocate<Decal>())Decal(material, size, maxDistance);
        SPtr<Decal> handlerPtr = te_core_ptr<Decal>(handler);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

        return handlerPtr;
    }

    SPtr<Decal> Decal::CreateEmpty()
	{
		Decal* decal = new (te_allocate<Decal>()) Decal();
		SPtr<Decal> decalPtr = te_core_ptr<Decal>(decal);
		decalPtr->SetThisPtr(decalPtr);

		return decalPtr;
	}
}
