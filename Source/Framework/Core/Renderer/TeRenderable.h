#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"
#include "Mesh/TeMesh.h"
#include "Math/TeMatrix4.h"

namespace te
{
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

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

        /**
         * Determines the mesh to render. All sub-meshes of the mesh will be rendered, and you may set individual materials
         * for each sub-mesh.
         */
        void SetMesh(SPtr<Mesh> mesh);

        /**	@copydoc SetMesh() */
        SPtr<Mesh> GetMesh() const { return _mesh; }

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
        UINT32 _rendererId;
        Matrix4 _tfrmMatrix = TeIdentity;
        Matrix4 _tfrmMatrixNoScale = TeIdentity;
        float _cullDistanceFactor = 1.0f;
    };
}
