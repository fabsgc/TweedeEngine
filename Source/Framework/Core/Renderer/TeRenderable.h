#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"
#include "Mesh/TeMesh.h"
#include "Math/TeMatrix4.h"
#include "Material/TeMaterial.h"

namespace te
{
    struct RenderableProperties
    {
        RenderableProperties()
        { }

        bool Instancing  = false;
        bool CanBeMerged = false;
        bool CastShadow = true;
        bool UseForDynamicEnvMapping  = true;
        float CullDistanceFactor = 1.0f;
    };

    /** Illuminates a portion of the scene covered by the Renderable. */
    class TE_CORE_EXPORT Renderable : public CoreObject, public SceneActor
    {
    public:
        Renderable();
        ~Renderable();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc SceneActor::Destroy */
        void SetMobility(ObjectMobility mobility) override;

        /** @copydoc CoreObject::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

        /** Determines the mesh to render. All sub-meshes of the mesh will be rendered, and you may set individual materials for each sub-mesh. */
        void SetMesh(SPtr<Mesh> mesh);

        /**	@copydoc SetMesh() */
        SPtr<Mesh> GetMesh() const { return _mesh; }

        /**
         * Sets a material that will be used for rendering a sub-mesh with the specified index. If a sub-mesh doesn't have
         * a specific material set then the primary material will be used.
         */
        void SetMaterial(UINT32 idx, const SPtr<Material>& material);

        /**
         * Sets the primary material to use for rendering. Any sub-mesh that doesn't have an explicit material set will use
         * this material.
         *
         * @note	This is equivalent to calling setMaterial(0, material).
         */
        void SetMaterial(const SPtr<Material>& material);

        /**
         * Sets a material, given its name in parameter. It's useful when you export a model and you want to set quickly
         * a material per submeshes
         *
         * Be careful, submeshes with another material will not be assigned. A default material will be applied by renderer
         */
        void SetMaterial(const String& name, const SPtr<Material>& material);

        /** @copydoc SetMaterials() */
        const Vector<SPtr<Material>>& GetMaterials() { return _materials; }

        const SPtr<Material>* GetMaterialsPtr() { return _materials.data(); }

        UINT32 GetNumMaterials() { return (UINT32)_materials.size(); }

        /**	Returns the material used for rendering a sub-mesh with the specified index. */
        SPtr<Material> GetMaterial(UINT32 idx) const;

        /**
         * Determines all materials used for rendering this renderable. Each of the materials is used for rendering a single
         * sub-mesh. If number of materials is larger than number of sub-meshes, they will be ignored. If lower, the
         * remaining materials will be removed.
         */
        void SetMaterials(const Vector<SPtr<Material>>& materials);

        /**
         * Determines the layer bitfield that controls whether a renderable is considered visible in a specific camera.
         * Renderable layer must match camera layer in order for the camera to render the component.
         */
        void SetLayer(UINT64 layer);

        /** Determines if this object can be instanced */
        void SetInstancing(bool instancing) { _properties.Instancing = instancing; _markCoreDirty(); }

        /** @copydoc SetInstancing() */
        bool GetInstancing() const { return  _properties.Instancing; }

        /** Determines if this object can be merged with another */
        void SetCanBeMerged(bool merged) { _properties.CanBeMerged = merged; _markCoreDirty(); }

        /** @copydoc SetInstancing() */
        bool GetCanBeMerged() const { return _properties.CanBeMerged; }

        /** Determines if this object cast shadow */
        void SetCastShadow(bool castShadow) { _properties.CastShadow = castShadow; _markCoreDirty(); }

        /** @copydoc SetCastShadow() */
        bool GetCastShadow() const { return _properties.CastShadow; }

        /** Set whole properties in a row */
        void SetPorperties(RenderableProperties& properties) { _properties = properties; _markCoreDirty(); }

        /** @copydoc SetPorperties() */
        const RenderableProperties& GetProperties() { return _properties; }

        /** @copydoc SetLayer() */
        UINT64 GetLayer() const { return _layer; }

        /** Factor to be applied to the cull distance set in the camera's render settings.  */
        void SetCullDistanceFactor(float factor);

        /** @copydoc SetCullDistanceFactor() */
        float GetCullDistanceFactor() const { return _cullDistanceFactor; }

        /**	Returns the transform matrix that is applied to the object when its being rendered. */
        Matrix4 GetMatrix() const { return _tfrmMatrix; }

        /**
         * Returns the transform matrix that is applied to the object when its being rendered. This transform matrix does
         * not include scale values.
         */
        Matrix4 GetMatrixNoScale() const { return _tfrmMatrixNoScale; }

        /**	Gets world bounds of the mesh rendered by this object. */
        Bounds GetBounds() const;

        /**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
        void SetRendererId(UINT32 id) { _rendererId = id; }

        /**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
        UINT32 GetRendererId() const { return _rendererId; }

        /** Triggered whenever the renderable's mesh changes. */
        void OnMeshChanged();

         /** @copydoc SceneActor::_updateState */
        void _updateState(const SceneObject& so, bool force = false) override;

        /**	Creates a new renderable handler instance. */
        static SPtr<Renderable> Create();

        /**	Creates a new renderable instance without initializing it. */
        static SPtr<Renderable> CreateEmpty();

    protected:
        SPtr<Mesh> _mesh;
        Vector<SPtr<Material>> _materials;
        UINT32 _numMaterials = 0;
        UINT64 _layer = 1;
        UINT32 _rendererId;
        Matrix4 _tfrmMatrix = TeIdentity;
        Matrix4 _tfrmMatrixNoScale = TeIdentity;
        float _cullDistanceFactor = 1.0f;

        RenderableProperties _properties;

        
        bool _instancing = false;
        bool _canBeMerged = false;
    };
}
