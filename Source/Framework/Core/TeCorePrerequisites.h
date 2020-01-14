#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "TeEngineConfig.h"

/** Maximum number of color surfaces that can be attached to a multi render target. */
#define TE_MAX_MULTIPLE_RENDER_TARGETS 8
#define TE_FORCE_SINGLETHREADED_RENDERING 0

/** Maximum number of individual GPU queues, per type. */
#define TE_MAX_QUEUES_PER_TYPE 8

/** Maximum number of hardware devices usable at once. */
#define TE_MAX_DEVICES 5U

/** Maximum number of devices one resource can exist at the same time. */
#define TE_MAX_LINKED_DEVICES 4U

// DLL export
#if TE_PLATFORM == TE_PLATFORM_WIN32 // Windows
#  if TE_COMPILER == TE_COMPILER_MSVC
#    if defined(TE_STATIC_LIB)
#      define TE_CORE_EXPORT
#    else
#      if defined(TE_CORE_EXPORTS)
#        define TE_CORE_EXPORT __declspec(dllexport)
#      else
#        define TE_CORE_EXPORT __declspec(dllimport)
#      endif
#	 endif
#  else
#    if defined(TE_STATIC_LIB)
#      define TE_CORE_EXPORT
#    else
#      if defined(TE_CORE_EXPORTS)
#        define TE_CORE_EXPORT __attribute__ ((dllexport))
#      else
#        define TE_CORE_EXPORT __attribute__ ((dllimport))
#      endif
#	 endif
#  endif
#  define TE_CORE_HIDDEN
#else // Linux/Mac settings
#  define TE_CORE_EXPORT __attribute__ ((visibility ("default")))
#  define TE_CORE_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

namespace te
{
    class CoreApplication;
    struct START_UP_DESC;

    class RenderAPI;
    class HardwareBuffer;
    class IndexBuffer;
    class VertexBuffer;
    class VertexDeclaration;
    class RenderTarget;
    class RenderWindow;
    class RenderAPIFactory;
    class RenderWindow;
    struct RENDER_WINDOW_DESC;
    class VideoMode;
    class VideoOutputInfo;
    class VideoModeInfo;
    class RenderWindowProperties;
    struct DEPTH_STENCIL_STATE_DESC;
    class DepthStencilState;
    struct PIPELINE_STATE_DESC;
    class DepthStencilProperties;
    class GraphicsPipelineState;
    struct RASTERIZER_STATE_DESC;
    class RasterizerProperties;
    class RasterizerState;
    struct RENDER_TARGET_BLEND_STATE_DESC;
    struct BLEND_STATE_DESC;
    class BlendProperties;
    class BlendState;
    struct SAMPLER_STATE_DESC;
    class SampleProperties;
    class SamplerState;
    class HardwareBuffer;
    class VertexBuffer;
    class VertexDeclaration;
    class VertexDataDesc;
    struct SubMesh;
    class TextureView;
    class HardwareBuffer;

    class Renderer;
    class RendererFactory;
    class CameraBase;
    class Camera;
    class ViewportBase;
    class Viewport;

    class SceneActor;

    class CoreObject;
    class CoreObjectManager;

    class RenderStateManager;

    class Input;
    class GamePad;
    class Keyboard;
    class Mouse;
    struct VIRTUAL_BUTTON_DESC;
    struct VIRTUAL_AXIS_DESC;
    class VirtualButton;
    class VirtualAxis;
    class InputConfiguration;
    class VirtualInput;

    class Win32Window;

    template<typename PluginFactory, class ...Args>
    class PluginManager;
    class RenderAPIManager;
    class RendererManager;

    class Platform;
    struct OSPointerButtonStates;

    class Resource;
    class ResourceManager;
    class GpuResourceData;

    class Texture;

    struct MESH_DESC;
    class MeshProperties;
    class Mesh;

    class Shader;
}

#include "Resources/TeResourceHandle.h"

namespace te
{
    typedef ResourceHandle<Resource> HResource;
    typedef ResourceHandle<Texture> HTexture;
    typedef ResourceHandle<Shader> HShader;
    typedef ResourceHandle<Mesh> HMesh;
    
}

#include "RenderAPI/TeCommonTypes.h"