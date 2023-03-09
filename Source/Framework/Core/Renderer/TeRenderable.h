#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"
#include "Math/TeBounds.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    struct EvaluatedAnimationData;

    struct RenderableProperties
    {
        bool Instancing  = false;
        bool CanBeMerged = false;
        bool CastShadows = true;
        bool CastLights = true;
        bool ReceiveShadows = true;
        bool UseForZPrepass = false;
        bool UseForLightProbes  = false;
        bool WriteVelocity = true;
        float CullDistanceFactor = 1.0f;
    };

    /** Type of animation that can be applied to a renderable object. */
    enum class RenderableAnimType
    {
        None,
        Skinned,
        Count // Keep at end
    };

    /** Illuminates a portion of the scene covered by the Renderable. */
    class TE_CORE_EXPORT Renderable : public CoreObject, public SceneActor, public Serializable
    {
    public:
        virtual ~Renderable();

        /** @copydoc SceneActor::Destroy */
        void SetMobility(ObjectMobility mobility) override;

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

        /**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
        void SetRendererId(UINT32 id) { _rendererId = id; }

        /**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
        UINT32 GetRendererId() const { return _rendererId; }

        /** Determines the mesh to render. All sub-meshes of the mesh will be rendered, and you may set individual materials for each sub-mesh. */
        void SetMesh(SPtr<Mesh> mesh);

        /** For Z Prepass, we want to use a lighter mesh. */
        void SetZPrepassMesh(SPtr<ZPrepassMesh> mesh);

        /**	@copydoc SetMesh */
        SPtr<Mesh> GetMesh() const { return _mesh; }

        /**	@copydoc SetMesh */
        SPtr<ZPrepassMesh> GetZPrepassMesh() const { return _ZPrepassMesh; }

        /**
         * Sets a material that will be used for rendering a sub-mesh with the specified index. If a sub-mesh doesn't have
         * a specific material set then the primary material will be used.
         */
        void SetMaterial(UINT32 idx, const SPtr<Material>& material);

        /**
         * Sets the primary material to use for rendering. If all is set to true, any sub-mesh that doesn't have 
         * an explicit material set will use this material.
         *
         * @note	This is equivalent to calling setMaterial(0, material).
         */
        void SetMaterial(const SPtr<Material>& material, bool all = false);

        /**
         * Sets a material, given its name in parameter. It's useful when you export a model and you want to set quickly
         * a material per submeshes
         *
         * Be careful, submeshes with another material will not be assigned. A default material will be applied by renderer
         */
        void SetMaterial(const String& name, const SPtr<Material>& material);

        /**
         * Determines all materials used for rendering this renderable. Each of the materials is used for rendering a single
         * sub-mesh. If number of materials is larger than number of sub-meshes, they will be ignored. If lower, the
         * remaining materials will be removed.
         */
        void SetMaterials(const Vector<SPtr<Material>>& materials);

        /** @copydoc SetMaterials */
        const Vector<SPtr<Material>>& GetMaterials() { return _materials; }

        const SPtr<Material>* GetMaterialsPtr() { return _materials.data(); }

        /**	Returns the material used for rendering a sub-mesh with the specified index. */
        SPtr<Material> GetMaterial(UINT32 idx) const;

        UINT32 GetNumMaterials() { return (UINT32)_materials.size(); }

        /** Remove all the instances of this material used on submesh for this renderable */
        void RemoveMaterial(const SPtr<Material>& material);

        /** Remove all materials set on every submesh */
        void ClearAllMaterials();

        /** We sometimes need to know if a material is currently used by a part of a renderable */
        bool IsUsingMaterial(const SPtr<Material>& material);

        /** If a material used by this renderable has been modified, we need to send the notification and refresh gpu params */
        void UpdateMaterials();

        /**
         * Determines the layer bitfield that controls whether a renderable is considered visible in a specific camera.
         * Renderable layer must match camera layer in order for the camera to render the component.
         */
        void SetLayer(UINT64 layer);

        /** @copydoc SetLayer */
        UINT64 GetLayer() const { return _layer; }

        /**
         * If enabled this renderable will write per-pixel velocity information when rendered. This is required for effects
         * such as temporal anti-aliasing and motion blur, but comes with a minor performance overhead. If you are not using
         * those effects you can disable this for a performance gain.
         */
        void SetWriteVelocity(bool enable);

        /** @copydoc setWriteVelocity */
        bool GetWriteVelocity() const { return _properties.WriteVelocity; }

        /** Determines if this object can be instanced */
        void SetInstancing(bool instancing) { _properties.Instancing = instancing; _markCoreDirty(); }

        /** @copydoc SetInstancing */
        bool GetInstancing() const { return  _properties.Instancing; }

        /** Determines if this object can be merged with another */
        void SetCanBeMerged(bool merged) { _properties.CanBeMerged = merged; _markCoreDirty(); }

        /** @copydoc SetInstancing */
        bool GetCanBeMerged() const { return _properties.CanBeMerged; }

        /** Determines if this object can cast shadows */
        void SetCastShadows(bool castShadows) { _properties.CastShadows = castShadows; _markCoreDirty(); }

        /** @copydoc SetCastShadows */
        bool GetCastShadows() const { return _properties.CastShadows; }

        /** Determines if this object can receive shadows */
        void SetReceiveShadows(bool receiveShadows) { _properties.ReceiveShadows = receiveShadows; _markCoreDirty(); }

        /** @copydoc SetCastShadows */
        bool GetReceiveShadows() const { return _properties.ReceiveShadows; }

        /** Determines if this object can cast lights */
        void SetCastLight(bool castLights) { _properties.CastLights = castLights; _markCoreDirty(); }

        /** @copydoc SetCastLights */
        bool GetCastLights() const { return _properties.CastLights; }

        /** Determines if this object is used for Z Prepass */
        void SetUseForZPrepass(bool useForZPrepass) { _properties.UseForZPrepass = useForZPrepass; _markCoreDirty(); }

        /** @copydoc SetUseForZPrepass */
        bool GetUseForZPrepass() const { return _properties.UseForZPrepass; }

        /** Set whole properties in a row */
        void SetPorperties(RenderableProperties& properties) { _properties = properties; _markCoreDirty(); }

        /** @copydoc SetPorperties */
        const RenderableProperties& GetProperties() { return _properties; }

        /** Factor to be applied to the cull distance set in the camera's render settings.  */
        void SetCullDistanceFactor(float factor);

        /** @copydoc SetCullDistanceFactor */
        float GetCullDistanceFactor() const { return _properties.CullDistanceFactor; }

        /** This object can be used for light probes generation. */
        void SetUseForLightProbes(bool use);

        /** @copydoc SetUseForLightProbes */
        bool GetUseForLightProbes() const { return _properties.UseForLightProbes; }

        /**	Returns the transform matrix that is applied to the object when its being rendered. */
        Matrix4 GetMatrix() const { return _tfrmMatrix; }

        /**
         * Returns the transform matrix that is applied to the object when its being rendered. This transform matrix does
         * not include scale values.
         */
        const Matrix4& GetMatrixNoScale() const { return _tfrmMatrixNoScale; }

        /**	Gets world bounds of the mesh rendered by this object. */
        Bounds GetBounds();

        /** Get subMesh specific bounding box */
        Bounds GetSubMeshBounds(UINT32 subMeshIdx = 0);

        /** Determines the animation that will be used for animating the attached mesh. */
        void SetAnimation(const SPtr<Animation>& animation);

        /** @copydoc setAnimation */
        const SPtr<Animation>& GetAnimation() const { return _animation; }

        /** Checks is the renderable animated or static. */
        bool IsAnimated() const { return _animation != nullptr; }

        /** Returns the type of animation influencing this renderable, if any. */
        RenderableAnimType GetAnimType() const { return _animType; }

        /** Returns the identifier of the animation, if this object is animated using skeleton or blend shape animation. */
        UINT64 GetAnimationId() const { return _animationId; }

        /**
         * Updates internal animation buffers from the contents of the provided animation data object. Does nothing if
         * renderable is not affected by animation.
         */
        void UpdateAnimationBuffers(const EvaluatedAnimationData& animData);

        /**
         * Records information about previous frame's animation buffer data. Should be called once per frame, before the
         * call to UpdateAnimationBuffers().
         */
        void UpdatePrevFrameAnimationBuffers();

        /** Returns the GPU buffer containing element's bone matrices, if it has any. */
        const SPtr<GpuBuffer>& GetBoneMatrixBuffer() const { return _boneMatrixBuffer; }

        /** Returns the GPU buffer containing element's bone matrices for the previous frame, if it has any. */
        const SPtr<GpuBuffer>& GetBonePrevMatrixBuffer() const { return _bonePrevMatrixBuffer; }

        /** Triggered whenever the renderable's mesh changes. */
        void OnMeshChanged();

         /** @copydoc SceneActor::UpdateState */
        void UpdateState(const SceneObject& so, bool force = false) override;

        /**
         * You can change at runtime which renderer will handle this renderable
         * Current renderer will be notified that renderable must be removed
         * And next renderer will be notified that renderable must be added
         */
        void AttachTo(SPtr<Renderer> renderer = nullptr);

        /**	Creates a new renderable handler instance. */
        static SPtr<Renderable> Create();

        /**	Creates a new renderable instance without initializing it. */
        static SPtr<Renderable> CreateEmpty();

    protected:
        friend class CRenderable;

        Renderable();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

    protected:
        /** Updates animation properties depending on the current mesh. */
        void RefreshAnimation();

        /** Creates any buffers required for renderable animation. Should be called whenever animation properties change. */
        void CreateAnimationBuffers();

        /** @copydoc SceneActor::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    protected:
        RenderableProperties _properties;

        SPtr<Mesh> _mesh;
        SPtr<ZPrepassMesh> _ZPrepassMesh;
        Vector<SPtr<Material>> _materials;
        UINT32 _numMaterials = 0;
        UINT64 _layer = 1;

        Matrix4 _tfrmMatrix = TeIdentity;
        Matrix4 _tfrmMatrixNoScale = TeIdentity;

        // Animation
        RenderableAnimType _animType = RenderableAnimType::None;
        SPtr<Animation> _animation;
        UINT64 _animationId = (UINT64)-1;
        SPtr<GpuBuffer> _boneMatrixBuffer;
        SPtr<GpuBuffer> _bonePrevMatrixBuffer;

        // Bounds must updated if we change _mesh or _tfrmMatrix
        Bounds _cachedBounds;
        bool _boundsDirty = true; 

        // For rendering sorting, it's better to use subMesh bounds instead of mesh bounds
        Vector<Bounds> _subMeshesBounds;
        bool _subMeshesBoundsDirty = true;

        UINT32 _rendererId = 0;
        SPtr<Renderer> _renderer; /** Default renderer if this attributes is not filled in constructor. */
    };
}
