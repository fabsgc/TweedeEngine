#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Math/TeRect2.h"
#include "Math/TeRect2I.h"
#include "Math/TeVector2I.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Renderer/TeRendererMaterial.h"

namespace te
{
    /**
     * Contains various utility methods that make various common operations in the renderer easier.
     */
    class TE_CORE_EXPORT RendererUtility : public Module<RendererUtility>
    {
    public:
        struct RenderTextureData
        {
            TEXTURE_DESC TargetColorDesc;
            TEXTURE_DESC TargetDepthDesc;
            RENDER_TEXTURE_DESC RenderTexDesc;
            HTexture ColorTex;
            HTexture DepthStencilTex;
            SPtr<RenderTexture> RenderTex;
            TextureSurface ColorTexSurface;
            UINT32 Width = 640;
            UINT32 Height = 480;
        };

    public:
        RendererUtility();
        virtual ~RendererUtility() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(RendererUtility)

        /**
         * Activates the specified material pass for rendering. Any further draw calls will be executed using this pass.
         *
         * @param[in]	material		Material containing the pass.
         * @param[in]	passIdx			Index of the pass in the material.
         * @param[in]	techniqueIdx	Index of the technique the pass belongs to, if the material has multiple techniques.
         */
        void SetPass(const SPtr<Material>& material, UINT32 passIdx, UINT32 techniqueIdx);

        /**
         * Activates the specified material pass for compute. Any further dispatch calls will be executed using this pass.
         *
         * @param[in]	material		Material containing the pass.
         * @param[in]	passIdx			Index of the pass in the material.
         */
        void SetComputePass(const SPtr<Material>& material, UINT32 passIdx = 0);

        /**
         * Sets parameters (textures, samplers, buffers) for the currently active pass.
         *
         * @param[in]	params		        Object containing the parameters.
         * @param[in]	gpuParamsBindFlags	Specify which parameters are binded to GPU
         * @param[in]	isInstanced     	Check if current object is instanced or not (more param buffer to update)
         */
        void SetPassParams(const SPtr<GpuParams> gpuParams, UINT32 gpuParamsBindFlags, bool isInstanced);

        /**
         * Draws the specified mesh.
         *
         * @param[in]	mesh			Mesh to draw.
         * @param[in]	numInstances	Number of times to draw the mesh using instanced rendering.
         */
        void Draw(const SPtr<Mesh>& mesh, UINT32 numInstances = 1);

        /**
         * Draws the specified mesh.
         *
         * @param[in]	mesh			Mesh to draw.
         * @param[in]	subMesh			Portion of the mesh to draw.
         * @param[in]	numInstances	Number of times to draw the mesh using instanced rendering.
         */
        void Draw(const SPtr<Mesh>& mesh, const SubMesh& subMesh, UINT32 numInstances = 1);

        /**
         * Draws a quad over the entire viewport in normalized device coordinates.
         *
         * @param[in]	uv				UV coordinates to assign to the corners of the quad.
         * @param[in]	textureSize		Size of the texture the UV coordinates are specified for. If the UV coordinates are
         *								already in normalized (0, 1) range then keep this value as is. If the UV coordinates
         *								are in texels then set this value to the texture size so they can be normalized
         *								internally.
         * @param[in]	numInstances	How many instances of the quad to draw (using instanced rendering). Useful when
         *								drawing to 3D textures.
         * @param[in]	flipUV			If true, vertical UV coordinate will be flipped upside down.
         */
        void DrawScreenQuad(const Rect2& uv, const Vector2I& textureSize = Vector2I(1, 1),
            UINT32 numInstances = 1, bool flipUV = false);

        /**
         * Draws a quad over the entire viewport in normalized device coordinates.
         *
         * @param[in]	numInstances	How many instances of the quad to draw (using instanced rendering). Useful when
         *								drawing to 3D textures.
         */
        void DrawScreenQuad(UINT32 numInstances = 1)
        {
            Rect2 uv(0.0f, 0.0f, 1.0f, 1.0f);
            Vector2I textureSize(1, 1);

            DrawScreenQuad(uv, textureSize, numInstances);
        }

        /** Returns a unit sphere stencil mesh. */
        SPtr<Mesh> GetSphereStencil() const { return _unitSphereStencilMesh; }

        /** Returns a unit axis aligned box stencil mesh. */
        SPtr<Mesh> GetBoxStencil() const { return _unitBoxStencilMesh; }

        /**
         * Returns a stencil mesh used for a spot light. Actual vertex positions need to be computed in shader as this
         * method will return uninitialized vertex positions.
         */
        SPtr<Mesh> GetSpotLightStencil() const { return _spotLightStencilMesh; }

        /**
         * Blits contents of the provided texture into the currently bound render target. If the provided texture contains
         * multiple samples, they will be resolved.
         *
         * @param[in]	texture	Source texture to blit.
         * @param[in]	area	Area of the source texture to blit in pixels. If width or height is zero it is assumed
         *						the entire texture should be blitted.
         * @param[in]	flipUV	If true, vertical UV coordinate will be flipped upside down.
         * @param[in]	isDepth	If true, the input texture is assumed to be a depth texture (instead of a color one).
         *						Multisampled depth textures will be resolved by taking the minimum value of all samples,
         *						unlike color textures which wil be averaged.
         */
        void Blit(const SPtr<Texture>& texture, const Rect2I& area = Rect2I::EMPTY, bool flipUV = false,
            bool isDepth = false);

        /** Returns a mesh that can be used for rendering a skybox. */
        SPtr<Mesh> GetSkyBoxMesh() const { return _skyBoxMesh; }

        /** Do a frustum culling on a renderable. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const SPtr<CRenderable> renderable);

        /** Do a frustum culling on a light. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const SPtr<CLight> light);

        /** Do a frustum culling on a scene camera. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const SPtr<CCamera> sceneCamera);

        /** Do a frustum culling on an audio listener. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const SPtr<CAudioListener> audio);

        /** Do a frustum culling on an audio source. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const SPtr<CAudioSource> audio);

        /** Do a frustum culling on an rigid body. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const SPtr<CRigidBody> rigidBody);

        /** Do a frustum culling on an soft body. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const SPtr<CSoftBody> softBody);

        /** Do a frustum culling on a renderable. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HRenderable& renderable);

        /** Do a frustum culling on a light. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HLight& light);

        /** Do a frustum culling on a scene camera. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HCamera& sceneCamera);

        /** Do a frustum culling on an audio listener. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HAudioListener& audio);

        /** Do a frustum culling on an audio source. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HAudioSource& audio);

        /** Do a more generic frustum culling */
        bool DoFrustumCulling(const HCamera& camera, const Sphere& boundingSphere, const float& cullDistanceFactor);

        /** Do a more generic frustum culling */
        bool DoFrustumCulling(const HCamera& camera, const AABox& boundingBox, const float& cullDistanceFactor);

        /** Generate a render texture using data given in parameters */
        void GenerateViewportRenderTexture(RenderTextureData& renderData, const String& debugName = String());

    private:
        static constexpr UINT32 NUM_QUAD_VB_SLOTS = 1024;

        SPtr<IndexBuffer> _fullScreenQuadIB;
        SPtr<VertexBuffer> _fullScreenQuadVB;
        SPtr<VertexDataDesc> _fullscreenQuadVDesc;
        SPtr<VertexDeclaration> _fullscreenQuadVDecl;
        UINT32 _nextQuadVBSlot = 0;

        SPtr<Mesh> _unitSphereStencilMesh;
		SPtr<Mesh> _unitBoxStencilMesh;
		SPtr<Mesh> _spotLightStencilMesh;
        SPtr<Mesh> _skyBoxMesh;
    };

    /** Provides easy access to RendererUtility. */
    TE_CORE_EXPORT RendererUtility& gRendererUtility();
}
