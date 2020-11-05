#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeGpuResourcePool.h"
#include "RenderAPI/TeRenderTexture.h"

namespace te
{
    struct SceneInfo;
    class RendererViewGroup;
    class RenderCompositorNode;
    struct FrameInfo;

    /** Inputs provided to each node in the render compositor hierarchy */
    struct RenderCompositorNodeInputs
    {
        RenderCompositorNodeInputs(const RendererViewGroup& viewGroup, const RendererView& view, const SceneInfo& scene,
            const RenderManOptions& options, const FrameInfo& frameInfo)
            : ViewGroup(viewGroup)
            , View(view)
            , Scene(scene)
            , Options(options)
            , FrameInfos(frameInfo)
        { }

        const RendererViewGroup& ViewGroup;
        const RendererView& View;
        const SceneInfo& Scene;
        const RenderManOptions& Options;
        const FrameInfo& FrameInfos;

        // Callbacks to external systems can hook into the compositor
        Vector<RenderCompositorNode*> InputNodes;
    };

    /**
     * Node in the render compositor hierarchy. Nodes can be implemented to perform specific rendering tasks. Each node
     * can depend on other nodes in the hierarchy.
     *
     * @note	Implementations must provide a GetNodeId() and GetDependencies() static method, which are expected to
     *			return a unique name for the implemented node, as well as a set of nodes it depends on.
     */
    class RenderCompositorNode
    {
        public:
        virtual ~RenderCompositorNode() = default;

    protected:
        friend class RenderCompositor;

        /** Executes the task implemented in the node. */
        virtual void Render(const RenderCompositorNodeInputs& inputs) = 0;

        /**
         * Cleans up any temporary resources allocated in a render() call. Any resources lasting longer than one frame
         * should be kept alive and released in some other manner.
         */
        virtual void Clear() = 0;
    };

    /**
     * Performs rendering by iterating over a hierarchy of render nodes. Each node in the hierarchy performs a specific
     * rendering tasks and passes its output to the dependant node. The system takes care of initializing, rendering and
     * cleaning up nodes automatically depending on their dependencies.
     */
    class RenderCompositor
    {
    public:
        struct NodeType;

    private:
        /** Contains internal information about a single render node. */
        struct NodeInfo
        {
            RenderCompositorNode* Node;
            NodeType* Type;
            UINT32 LastUseIdx;
            Vector<RenderCompositorNode*> Inputs;
        };

    public:
        ~RenderCompositor();

        /**
         * Rebuilds the render node hierarchy. Call this whenever some setting that may influence the render node
         * dependencies changes.
         *
         * @param[in]	view		Parent view to which this compositor belongs to.
         * @param[in]	finalNode	Identifier of the final node in the node hierarchy. This node is expected to write
         *							to the views render target. All other nodes will be deduced from this node's
         *							dependencies.
         */
        void Build(const RendererView& view, const String& finalNode);

        /** Performs rendering using the current render node hierarchy. This is expected to be called once per frame. */
        void Execute(RenderCompositorNodeInputs& inputs) const;

    private:
        /** Clears the render node hierarchy. */
        void Clear();

        Vector<NodeInfo> _nodeInfos;
        bool _isValid = false;

        /************************************************************************/
        /* 							NODE TYPES	                     			*/
        /************************************************************************/
    public:
        /** Contains information about a specific node type. */
        struct NodeType
        {
            virtual ~NodeType() = default;

            /** Creates a new node of this type. */
            virtual RenderCompositorNode* Create() const = 0;

            /** Returns identifier for all the dependencies of a node of this type. */
            virtual Vector<String> GetDependencies(const RendererView& view) const = 0;

            String id;
        };
        
        /** Templated implementation of NodeType. */
        template<class T>
        struct TNodeType : NodeType
        {
            TNodeType()
            {
                id = T::GetNodeId();
            }

            /** @copydoc NodeType::Create() */
            RenderCompositorNode* Create() const override { return te_new<T>(); }

            /** @copydoc NodeType::GetDependencies() */
            Vector<String> GetDependencies(const RendererView& view) const override
            {
                return T::GetDependencies(view);
            }
        };

        /**
         * Registers a new type of node with the system. Each node type must first be registered before it can be used
         * in the node hierarchy.
         */
        template<class T>
        static void RegisterNodeType()
        {
            auto findIter = _nodeTypes.find(T::GetNodeId());
            if (findIter != _nodeTypes.end())
                TE_DEBUG("Found two render compositor nodes with the same name \"{" + String(T::GetNodeId().c_str()) + "}\".");

            _nodeTypes[T::GetNodeId()] = te_new<TNodeType<T>>();
        }

        /** Releases any information about render node types. */
        static void CleanUp()
        {
            for (auto& entry : _nodeTypes)
                te_delete(entry.second);

            _nodeTypes.clear();
        }

    private:
        static UnorderedMap<String, NodeType*> _nodeTypes;
    };

    /**
     * Initializes the GBuffer textures
     */
    class RCNodeGpuInitializationPass : public RenderCompositorNode
    {
    public:
        // Outputs
        SPtr<PooledRenderTexture> SceneTex;
        SPtr<PooledRenderTexture> AlbedoTex;
        SPtr<PooledRenderTexture> SpecularTex;
        SPtr<PooledRenderTexture> NormalTex;
        SPtr<PooledRenderTexture> VelocityTex;
        SPtr<PooledRenderTexture> EmissiveTex;
        SPtr<PooledRenderTexture> DepthTex;

        SPtr<RenderTexture> RenderTargetTex;

        static String GetNodeId() { return "GpuInitializationPass"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::clear */
        void Clear() override;
    };

    /**
     * Render all opaque objects using forward rendering technique
     */
    class RCNodeForwardPass : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "ForwardPass"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::clear */
        void Clear() override;
    };

    /**
     * Renders the skybox into the scene color texture. If skybox texture is not available, a solid color will be rendered
     * instead.
     */
    class RCNodeSkybox : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "Skybox"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::Clear */
        void Clear() override;
    };

    /**
     * Renders all transparent object
     */
    class RCNodeForwardTransparentPass : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "ForwardTransparentPass"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::clear */
        void Clear() override;
    };

    /************************************************************************/
    /* 							POST PROCESS NODES                			*/
    /************************************************************************/

    /**
     * Helper node used for post-processing. Takes care of allocating and switching between textures used for post process
     * effects.
     */
    class RCNodePostProcess : public RenderCompositorNode
    {
    public:
        /**
         * Returns a texture that can be used for rendering a post-process effect, and the result of the previous
         * output. Switches these textures so the next call they are returned in the opposite parameters.
         */
        void GetAndSwitch(const RendererView& view, SPtr<RenderTexture>& output, SPtr<Texture>& lastFrame) const;

        /** Returns a texture that contains the last rendererd post process output. */
        SPtr<Texture> GetLastOutput() const;

        static String GetNodeId() { return "PostProcess"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::Clear */
        void Clear() override;

    protected:
        mutable SPtr<PooledRenderTexture> _output[2];
        mutable UINT32 _currentIdx = 0;
    };

    /**
     * Performs tone mapping on the contents of the scene color texture. At the same time resolves MSAA into a non-MSAA
     * scene color texture.
     */
    class RCNodeTonemapping : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "Tonemapping"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::Clear */
        void Clear() override;

        SPtr<PooledRenderTexture> _tonemapLUT;
        UINT64 _tonemapLastUpdateHash = -1;
    };

    /**
     * Renders the motion blur effect simulating light accumulation due to object and/or camera
     * movement during a single frame. (In another words, it simulates blur due to exposure time
     * as if on a real-world camera, i.e. depending on how long is the camera shutter open).
     */
    class RCNodeMotionBlur : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "MotionBlur"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::clear */
        void Clear() override;
    };

    /** Renders the depth of field effect using Gaussian blurring. */
    class RCNodeGaussianDOF : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "GaussianDOF"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::clear */
        void Clear() override;
    };

    /** Renders FXAA. */
    class RCNodeFXAA : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "FXAA"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::Clear */
        void Clear() override;
    };

    /** Renders TAA. */
    class RCNodeTemporalAA : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "TAA"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::Clear */
        void Clear() override;
    };

    /** Renders screen space ambient occlusion. */
    class RCNodeSSAO : public RenderCompositorNode
    {
    public:
        SPtr<Texture> output;

        static String GetNodeId() { return "SSAO"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::Clear */
        void Clear() override;

        SPtr<PooledRenderTexture> _pooledOutput;
    };

    /** Renders the bloom effect. */
    class RCNodeBloom : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "Bloom"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::Clear */
        void Clear() override;

        SPtr<PooledRenderTexture> _pooledOutput;
    };

    /** Moves the contents of the scene color texture into the view's output target. */
    class RCNodeFinalResolve : public RenderCompositorNode
    {
    public:
        static String GetNodeId() { return "FinalResolve"; }
        static Vector<String> GetDependencies(const RendererView& view);

    protected:
        /** @copydoc RenderCompositorNode::Render */
        void Render(const RenderCompositorNodeInputs& inputs) override;

        /** @copydoc RenderCompositorNode::clear */
        void Clear() override;
    };
}
