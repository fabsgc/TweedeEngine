#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "RenderAPI/TeRenderTarget.h"
#include "RenderAPI/TeRenderWindow.h"
#include "RenderAPI/TeRenderAPICapabilities.h"
#include "Image/TeColor.h"

#define TE_MAX_BOUND_VERTEX_BUFFERS 16

namespace te
{
    class TE_CORE_EXPORT RenderAPI : public Module<RenderAPI>
    {
    public:
        RenderAPI();
        virtual ~RenderAPI();

        TE_MODULE_STATIC_HEADER_MEMBER(RenderAPI)

        virtual SPtr<RenderWindow> CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc) = 0;

        /** Initialize render API (depending on configuration). */
        virtual void Initialize();

        /** Shuts down the render API system and cleans up all resources. */
        virtual void Destroy();

        /**
         * Applies a set of parameters that control execution of all currently bound GPU programs. These are the uniforms
         * like textures, samplers, or uniform buffers. Caller is expected to ensure the provided parameters actually
         * match the currently bound programs.
         *
         * WARNING : Be careful when you set different way of binding buffers. All buffers you want to bind must be consecutives in memory
         * 
         * @param[in]	gpuParams					GpuPArams containing all elements to bind
         * @param[in]	gpuParamsBindFlags			Specify which kind of params you want to bind
         * @param[in]	gpuParamsBlockBindFlags		In case you only want to bind constant buffers, you can specify how there are selected
         * @param[in]	paramBlocksToBind			All constants buffers you want to bind by name. Be careful, buffers must to be consecutive in memory
         */
        virtual void SetGpuParams(const SPtr<GpuParams>& gpuParams, UINT32 gpuParamsBindFlags = (UINT32)GPU_BIND_ALL,
            UINT32 gpuParamsBlockBindFlags = (UINT32)GPU_BIND_PARAM_BLOCK_ALL, const Vector<String>& paramBlocksToBind = {}) = 0;

        /**
         * Sets a pipeline state that controls how will subsequent draw commands render primitives.
         *
         * @param[in]	pipelineState		Pipeline state to bind, or null to unbind.
         *
         * @see		GraphicsPipelineState
         */
        virtual void SetGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState) = 0;

        /**
         * Sets a pipeline state that controls how will subsequent dispatch commands execute.
         *
         * @param[in]	pipelineState		Pipeline state to bind, or null to unbind.
         */
        virtual void SetComputePipeline(const SPtr<ComputePipelineState>& pipelineState) = 0;

        /**
         * Sets the active viewport that will be used for all render operations.
         *
         * @param[in]	area			Area of the viewport, in normalized ([0,1] range) coordinates.
         */
        virtual void SetViewport(const Rect2& area) = 0;

        /**
         * Allows you to set up a region in which rendering can take place. Coordinates are in pixels. No rendering will be
         * done to render target pixels outside of the provided region.
         *
         * @param[in]	left			Left border of the scissor rectangle, in pixels.
         * @param[in]	top				Top border of the scissor rectangle, in pixels.
         * @param[in]	right			Right border of the scissor rectangle, in pixels.
         * @param[in]	bottom			Bottom border of the scissor rectangle, in pixels.
         */
        virtual void SetScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom) = 0;

        /**
         * Sets a reference value that will be used for stencil compare operations.
         *
         * @param[in]	value			Reference value to set.
         */
        virtual void SetStencilRef(UINT32 value) = 0;

        /**
         * Sets the provided vertex buffers starting at the specified source index.	Set buffer to nullptr to clear the
         * buffer at the specified index.
         *
         * @param[in]	index			Index at which to start binding the vertex buffers.
         * @param[in]	buffers			A list of buffers to bind to the pipeline.
         * @param[in]	numBuffers		Number of buffers in the @p buffers list.
         */
        virtual void SetVertexBuffers(UINT32 index, SPtr<VertexBuffer>* buffers, UINT32 numBuffers) = 0;

        /**
         * Sets an index buffer to use when drawing. Indices in an index buffer reference vertices in the vertex buffer,
         * which increases cache coherency and reduces the size of vertex buffers by eliminating duplicate data.
         *
         * @param[in]	buffer			Index buffer to bind, null to unbind.
         */
        virtual void SetIndexBuffer(const SPtr<IndexBuffer>& buffer) = 0;

        /**
         * Sets the vertex declaration to use when drawing. Vertex declaration is used to decode contents of a single
         * vertex in a vertex buffer.
         *
         * @param[in]	vertexDeclaration	Vertex declaration to bind.
         */
        virtual void SetVertexDeclaration(const SPtr<VertexDeclaration>& vertexDeclaration) = 0;

        /**
         * Sets the draw operation that determines how to interpret the elements of the index or vertex buffers.
         *
         * @param[in]	op				Draw operation to enable.
         */
        virtual void SetDrawOperation(DrawOperationType op) = 0;
        
        /**
         * Draw an object based on currently bound GPU programs, vertex declaration and vertex buffers. Draws directly from
         * the vertex buffer without using indices.
         *
         * @param[in]	vertexOffset	Offset into the currently bound vertex buffer to start drawing from.
         * @param[in]	vertexCount		Number of vertices to draw.
         * @param[in]	instanceCount	Number of times to draw the provided geometry, each time with an (optionally)
         */
        virtual void Draw(UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount = 0) = 0;

        /**
         * Draw an object based on currently bound GPU programs, vertex declaration, vertex and index buffers.
         *
         * @param[in]	startIndex		Offset into the currently bound index buffer to start drawing from.
         * @param[in]	indexCount		Number of indices to draw.
         * @param[in]	vertexOffset	Offset to apply to each vertex index.
         * @param[in]	vertexCount		Number of vertices to draw.
         * @param[in]	instanceCount	Number of times to draw the provided geometry, each time with an (optionally)
         */
        virtual void DrawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount = 0) = 0;

        /**
         * Executes the currently bound compute shader.
         *
         * @param[in]	numGroupsX		Number of groups to start in the X direction. Must be in range [1, 65535].
         * @param[in]	numGroupsY		Number of groups to start in the Y direction. Must be in range [1, 65535].
         * @param[in]	numGroupsZ		Number of groups to start in the Z direction. Must be in range [1, 64].
         */
        virtual void DispatchCompute(UINT32 numGroupsX, UINT32 numGroupsY = 1, UINT32 numGroupsZ = 1) = 0;

        /**
         * Swap the front and back buffer of the specified render target.
         * 
         * @param[in]	target		Render target to perform the buffer swap on.
         */
        virtual void SwapBuffers(const SPtr<RenderTarget>& target) = 0;

        /**
         * Change the render target into which we want to draw.
         * 
         * @param[in]	target					Render target to draw to.
         * @param[in]	readOnlyFlags			Combination of one or more elements of FrameBufferType denoting which buffers
         *										will be bound for read-only operations. This is useful for depth or stencil
         *										buffers which need to be bound both for depth/stencil tests, as well as
         *										shader reads.
         */
        virtual void SetRenderTarget(const SPtr<RenderTarget>& target, UINT32 readOnlyFlags = 0) = 0;

        /**
         * Clears the currently active render target.
         *
         * @param[in]	buffers			Combination of one or more elements of FrameBufferType denoting which buffers are
         *								to be cleared.
         * @param[in]	color			The color to clear the color buffer with, if enabled.
         * @param[in]	depth			The value to initialize the depth buffer with, if enabled.
         * @param[in]	stencil			The value to initialize the stencil buffer with, if enabled.
         * @param[in]	targetMask		In case multiple render targets are bound, this allows you to control which ones to
         *									clear (0x01 first, 0x02 second, 0x04 third, etc., and combinations).
         */
        virtual void ClearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f,
            UINT16 stencil = 0, UINT8 targetMask = 0xFF) = 0;

        /**
         * Clears the currently active viewport (meaning it clears just a sub-area of a render-target that is covered by the
         * viewport, as opposed to clearRenderTarget() which always clears the entire render target).
         *
         * @param[in]	buffers			Combination of one or more elements of FrameBufferType denoting which buffers are to
         *								be cleared.
         * @param[in]	color			The color to clear the color buffer with, if enabled.
         * @param[in]	depth			The value to initialize the depth buffer with, if enabled.
         * @param[in]	stencil			The value to initialize the stencil buffer with, if enabled.
         * @param[in]	targetMask		In case multiple render targets are bound, this allows you to control which ones to
         *								clear (0x01 first, 0x02 second, 0x04 third, etc., and combinations).
         */
        virtual void ClearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f,
            UINT16 stencil = 0, UINT8 targetMask = 0xFF) = 0;

        /** Returns information about available output devices and their video modes. */
        const VideoModeInfo& GetVideoModeInfo() const { return *_videoModeInfo; }

        /** Get capabilities for specified device */
        const RenderAPICapabilities& GetCapabilities(UINT32 deviceIdx = 0) const;

        /** @copydoc GetCapabilities */
        RenderAPICapabilities* GetCapabilitiesPtr(UINT32 deviceIdx = 0) const;

        /** Contains a default matrix into a matrix suitable for use by this specific render system. */
        virtual void ConvertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) = 0;

        /**
         * Generates a parameter block description and calculates per-parameter offsets for the provided gpu data
         * parameters. The offsets are render API specific and correspond to std140 layout for OpenGL, and the default
         * layout in DirectX.
         *
         * @param[in]	name	Name to assign the parameter block.
         * @param[in]	params	List of parameters in the parameter block. Only name, type and array size fields need to be
         * 						populated, the rest will be populated when the method returns. If a parameter is a struct
         * 						then the elementSize field needs to be populated with the size of the struct in bytes.
         * @return				Descriptor for the parameter block holding the provided parameters as laid out by the
         *						default render API layout.
         */
        virtual GpuParamBlockDesc GenerateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params) = 0;

        /** Returns available GPU memory in mega byte */
        virtual UINT64 GetGPUMemory() = 0;

        /** Returns available shared memory in mega byte */
        virtual UINT64 GetSharedMemory() = 0;

        /** Returns used GPU memory in mega byte */
        virtual UINT64 GetUsedGPUMemory() = 0;

        /** Converts the number of vertices to number of primitives based on the specified draw operation. */
        UINT32 VertexCountToPrimCount(DrawOperationType type, UINT32 elementCount);

        /** You can provide annotations to group regions of the frame under names, with a nested hierarchy */
        virtual void PushMarker(const String& name, const Color& color) const { }

        /** @copydoc PushMarker */
        virtual void PopMarker() const { }

    protected:
        SPtr<RenderTarget> _activeRenderTarget;
        bool _activeRenderTargetModified = false;

        SPtr<VideoModeInfo> _videoModeInfo;
        UINT32 _numDevices;
        RenderAPICapabilities* _capabilities;
    };

    /** Shorthand for RenderAPI::GetCapabilities(). */
    inline const RenderAPICapabilities& gCaps(UINT32 deviceIdx = 0)
    {
        return RenderAPI::Instance().GetCapabilities(deviceIdx);
    }

    inline RenderAPICapabilities* gCapsPtr(UINT32 deviceIdx = 0)
    {
        return RenderAPI::Instance().GetCapabilitiesPtr(deviceIdx);
    }
}
