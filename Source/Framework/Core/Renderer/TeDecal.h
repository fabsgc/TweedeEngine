#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Serialization/TeSerializable.h"
#include "Scene/TeSceneActor.h"
#include "Math/TeVector2.h"
#include "Math/TeBounds.h"

namespace te
{
    /**
     * Specifies a decal that will be projected onto scene geometry. User can set the material to use when rendering
     * the decal, as well as control decal orientation and size.
     */
    class TE_CORE_EXPORT Decal : public CoreObject, public SceneActor, public Serializable
    {
    public:
        virtual ~Decal();

        /**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
        void SetRendererId(UINT32 id) { _rendererId = id; }

        /**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
        UINT32 GetRendererId() const { return _rendererId; }

        /** Determines the material to use when rendering the decal. */
        void SetMaterial(const SPtr<Material>& material) { _material = material; _markCoreDirty(); }

        /** @copydoc setMaterial */
        const SPtr<Material>& GetMaterial() const { return _material; }

        /** Width and height of the decal. */
        void SetSize(const Vector2& size) { _size = Vector2::Max(Vector2::ZERO, size); _markCoreDirty(); UpdateBounds(); }

        /** @copydoc SetSize */
        Vector2 GetSize() const { return _size; }

        /** Returns width and height of the decal, scaled by decal's transform. */
        Vector2 GetWorldSize() const
        {
            return Vector2(_size.x * _transform.GetScale().x, _size.y * _transform.GetScale().y);
        }

        /** Determines the maximum distance (from its origin) at which the decal is displayed. */
        void SetMaxDistance(float distance) { _maxDistance = Math::Max(0.0f, distance); _markCoreDirty(); UpdateBounds(); }

        /** @copydoc GetSize */
        float GetMaxDistance() const { return _maxDistance; }

        /** Maximum distance (from its origin) at which the decal is displayed, scaled by decal's transform. */
        float GetWorldMaxDistance() const { return _maxDistance * _transform.GetScale().z; }

        /**
         * Bitfield that allows you to mask on which objects will the decal be projected onto. Only objects with the
         * matching layers will be projected onto. Note that decal layer mask only supports 32-bits and objects with
         * layers in bits >= 32 will always be projected onto.
         */
        void SetLayerMask(UINT32 mask) { _layerMask = mask; _markCoreDirty(); }

        /** @copydoc SetLayerMask */
        UINT32 GetLayerMask() const { return _layerMask; }

        /**
         * Determines the layer that controls whether a system is considered visible in a specific camera. Layer must match
         * camera layer bitfield in order for the camera to render the decal.
         */
        void SetLayer(UINT32 layer);

        /** @copydoc SetLayer() */
        UINT32 GetLayer() const { return _layer; }

        /**	Gets world bounds of this object. */
        Bounds GetBounds() const { return _bounds; }

        /**	Returns the transform matrix that is applied to the object when its being rendered. */
        Matrix4 GetMatrix() const { return _tfrmMatrix; }

        /**
         * Returns the transform matrix that is applied to the object when its being rendered. This transform matrix does
         * not include scale values.
         */
        Matrix4 GetMatrixNoScale() const { return _tfrmMatrixNoScale; }

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

        /**
         * You can change at runtime which renderer will handle this decal
         * Current renderer will be notified that decal must be removed
         * And next renderer will be notified that decal must be added
         */
        void AttachTo(SPtr<Renderer> renderer = nullptr);

        /**
         * Creates a new decal.
         *
         * @param[in]	material		Material to use when rendering the decal.
         * @param[in]	size			Size of the decal in world units.
         * @param[in]	maxDistance		Maximum distance at which will the decal be visible (from the current decal origin,
         *								along the negative Z axis).
         * @returns						New decal object.
         */
        static SPtr<Decal> Create(const HMaterial& material, const Vector2& size = Vector2::ONE, float maxDistance = 10.0f);

        /**	Creates the object with without initializing it. Used for serialization. */
        static SPtr<Decal> CreateEmpty();

    protected:
        /** Updates the internal bounds for the decal. Call this whenever a property affecting the bounds changes. */
        void UpdateBounds();

        /** @copydoc SceneActor::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    protected:
        friend class CDecal;

        Decal();
        Decal(const HMaterial& material, const Vector2& size, float maxDistance);
        Decal(const SPtr<Material>& material, const Vector2& size, float maxDistance);

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

    protected:
        Bounds _bounds;
        SPtr<Material> _material;
        Matrix4 _tfrmMatrix = TeIdentity;
        Matrix4 _tfrmMatrixNoScale = TeIdentity;
        float _maxDistance = 10.0f;
        Vector2 _size = Vector2::ONE;
        UINT32 _layer = 1;
        UINT32 _layerMask = 0xFFFFFFFF;

        UINT32 _rendererId = 0;
        SPtr<Renderer> _renderer; /** Default renderer if this attributes is not filled in constructor. */
    };
}
