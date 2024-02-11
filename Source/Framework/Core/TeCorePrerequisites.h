#pragma once

#include "TeEngineConfig.h"
#include "Prerequisites/TePrerequisitesUtility.h"

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
# endif
#  else
#    if defined(TE_STATIC_LIB)
#      define TE_CORE_EXPORT
#    else
#      if defined(TE_CORE_EXPORTS)
#        define TE_CORE_EXPORT __attribute__ ((dllexport))
#      else
#        define TE_CORE_EXPORT __attribute__ ((dllimport))
#      endif
#    endif
#  endif
#  define TE_CORE_HIDDEN
#else // Linux/Mac settings
#  define TE_CORE_EXPORT __attribute__ ((visibility ("default")))
#  define TE_CORE_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

#ifdef _ITERATOR_DEBUG_LEVEL
#undef _ITERATOR_DEBUG_LEVEL
#endif

namespace te
{
    enum TypeID_Core
    {
        TID_Component                 = 1001,
        TID_CCamera                   = 1002,
        TID_CRenderable               = 1003,
        TID_CLight                    = 1004,
        TID_CSkybox                   = 1005,
        TID_CCameraFlyer              = 1006,
        TID_CScript                   = 1007,
        TID_CCameraUI                 = 1008,
        TID_SceneObject               = 1008,
        TID_Texture                   = 1010,
        TID_Material                  = 1011,
        TID_Pass                      = 1012,
        TID_Shader                    = 1013,
        TID_Technique                 = 1014,
        TID_Mesh                      = 1015,
        TID_Font                      = 1016,
        TID_NativeScript              = 1017,
        TID_Resource                  = 1018,
        TID_Skeleton                  = 1019,
        TID_ImportOptions             = 1020,
        TID_FontImportOptions         = 1021,
        TID_TextureImportOptions      = 1022,
        TID_MeshImportOptions         = 1023,
        TID_ShaderImportOptions       = 1024,
        TID_AudioClipImportOptions    = 1025,
        TID_CBone                     = 1026,
        TID_CAnimation                = 1027,
        TID_Animation                 = 1028,
        TID_AnimationClip             = 1029,
        TID_AnimationSplitInfo        = 1030,
        TID_ImportedAnimationEvents   = 1031,
        TID_AudioClip                 = 1032,
        TID_AudioListener             = 1033,
        TID_AudioSource               = 1034,
        TID_CAudioListener            = 1035,
        TID_CAudioSource              = 1036,
        TID_CJoint                    = 1037,
        TID_CHingeJoint               = 1038,
        TID_CSliderJoint              = 1039,
        TID_CSphericalJoint           = 1040,
        TID_CD6Joint                  = 1041,
        TID_CConeTwistJoint           = 1042,
        TID_CRigidBody                = 1043,
        TID_CSoftBody                 = 1044,
        TID_CCollider                 = 1045,
        TID_CBoxCollider              = 1046,
        TID_CPlaneCollider            = 1047,
        TID_CSphereCollider           = 1048,
        TID_CCylinderCollider         = 1049,
        TID_CCapsuleCollider          = 1050,
        TID_CMeshCollider             = 1051,
        TID_CConeCollider             = 1052,
        TID_CHeightFieldCollider      = 1053,
        TID_CBody                     = 1054,
        TID_PhysicsMesh               = 1055,
        TID_FPhysicsMesh              = 1056,
        TID_PhysicsHeightField        = 1057,
        TID_FPhysicsHeightField       = 1058,
        TID_FBody                     = 1059,
        TID_FJoint                    = 1060,
        TID_FCollider                 = 1061,
        TID_RigidBody                 = 1062,
        TID_SoftBody                  = 1063,
        TID_HingeJoint                = 1064,
        TID_SliderJoint               = 1065,
        TID_SphericalJoint            = 1066,
        TID_D6Joint                   = 1067,
        TID_ConeTwistJoint            = 1068,
        TID_BoxCollider               = 1069,
        TID_PlaneCollider             = 1070,
        TID_SphereCollider            = 1071,
        TID_CylinderCollider          = 1072,
        TID_CapsuleCollider           = 1073,
        TID_MeshCollider              = 1074,
        TID_ConeCollider              = 1075,
        TID_HeightFieldCollider       = 1076,
        TID_MeshSoftBody              = 1077,
        TID_EllipsoidSoftBody         = 1078,
        TID_RopeSoftBody              = 1079,
        TID_PatchSoftBody             = 1080,
        TID_CMeshSoftBody             = 1081,
        TID_CEllipsoidSoftBody        = 1082,
        TID_CRopeSoftBody             = 1083,
        TID_CPatchSoftBody            = 1084,
        TID_FSoftBody                 = 1085,
        TID_Decal                     = 1086,
        TID_CDecal                    = 1087,
        TID_Light                     = 1088,
        TID_Renderable                = 1089,
        TID_Skybox                    = 1090,
        TID_ShaderVariation           = 1091,
        TID_ShaderVariationParam      = 1092,
        TID_ShaderVariationParamInfo  = 1093,
        TID_Camera                    = 1094,
        TID_ZPrepassMesh              = 1095,
        TID_ExportOptions             = 1096
    };
}

namespace te
{
    class CoreApplication;
    struct START_UP_DESC;

    class RenderAPI;
    class HardwareBuffer;
    class IndexBuffer;
    class VertexData;
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
    class ComputePipelineState;
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
    struct INDEX_BUFFER_DESC;
    class IndexBufferProperties;
    class IndexBuffer;
    struct VERTEX_BUFFER_DESC;
    class VerteBufferProperties;
    class VertexBuffer;
    class VertexElement;
    class VertexDeclarationProperties;
    class VertexDeclaration;
    class VertexDataDesc;
    struct SubMesh;
    class ShapeMeshes3D;
    class TextureView;
    class HardwareBuffer;
    struct RENDER_TEXTURE_DESC;
    struct RENDER_SURFACE_DESC;
    class RenderTexture;
    struct GpuProgramBytecode;
    struct GPU_PROGRAM_DESC;
    class GpuProgram;
    struct GpuParamDataDesc;
    struct GpuParamObjectDesc;
    struct GpuParamBlockDesc;
    struct GpuParamDesc;
    class GpuProgramManager;
    class HardwareBufferManager;
    struct GPU_PIPELINE_PARAMS_DESC;
    class GpuPipelineParamInfo;
    class GpuParamBlockBuffer;
    class GpuParams;
    struct GPU_BUFFER_DESC;
    class GpuBufferProperties;
    class GpuBuffer;
    class VertexDeclaration;
    class VertexBuffer;
    struct GpuParamDesc;
    struct GpuParamDataDesc;
    struct GpuParamBlockDesc;
    struct GpuParamObjectDesc;

    class Renderer;
    class RendererFactory;
    class CameraBase;
    class Camera;
    class ViewportBase;
    class Viewport;
    class Light;
    class Renderable;
    struct RenderableProperties;
    struct RenderSettings;
    class RenderQueue;
    class RenderElement;
    class Skybox;
    class RendererMaterialManager;
    class RendererMaterialBase;
    template <typename T>
    class RendererMaterial;
    class BlitMat;
    class GaussianBlurMat;
    class RendererMeshData;
    class Decal;
    class TextureDownsampleMat;
    class TextureCubeDownsampleMat;
    class IBLUtility;
    class GpuResourcePool;
    class Exposure;

    class SceneActor;
    class SceneManager;
    class SceneObject;
    class GameObject;
    class Component;
    template <typename T>
    class GameObjectHandle;
    class GameObjectManager;
    class Transform;

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
    class GuiManager;

    class Platform;
    struct OSPointerButtonStates;

    class Resource;
    class ResourceManager;
    class GpuResourceData;

    class Texture;
    class PixelData;
    class TextureAtlasLayout;
    class TextureAtlasUtility;
    class Color;
    struct PixelVolume;
    class TextureManager;

    struct MESH_DESC;
    class MeshProperties;
    class Mesh;
    class ZPrepassMesh;
    class MeshData;
    struct BoneWeight;

    class Shader;
    class Material;
    class MaterialProperties;
    class Technique;
    class Pass;
    struct SHADER_DESC;
    struct PASS_DESC;

    class Serializable;

    class CCamera;
    class CCameraFlyer;
    class CCameraUI;
    class CRenderable;
    class CLight;
    class CSkybox;
    class CScript;
    class CBone;
    class CAnimation;
    class CAudioSource;
    class CAudioListener;
    class CJoint;
    class CHingeJoint;
    class CSliderJoint;
    class CSphericalJoint;
    class CD6Joint;
    class CConeTwistJoint;
    class CBody;
    class CRigidBody;
    class CSoftBody;
    class CMeshSoftBody;
    class CEllipsoidSoftBody;
    class CRopeSoftBody;
    class CPatchSoftBody;
    class CCollider;
    class CBoxCollider;
    class CPlaneCollider;
    class CSphereCollider;
    class CCylinderCollider;
    class CCapsuleCollider;
    class CMeshCollider;
    class CConeCollider;
    class CHeightFieldCollider;
    class CDecal;

    class BuiltinResources;

    class Audio;
    struct AudioDevice;
    class AudioManager;
    class AudioClip;
    class AudioListener;
    class AudioSource;

    class Font;

    class Animation;
    class AnimationClip;
    template <class T> class TAnimationCurve;
    class Skeleton;

    class GuiAPI;
    class GuiAPIFactory;

    class Script;
    class NativeScript;
    class SceneManager;

    class Importer;
    class BaseImporter;
    struct SubResourceRaw;
    struct SubResourceUUID;
    struct SubResource;
    struct MultiResource;
    class ImportOptions;
    class MeshImportOptions;
    class ShaderImportOptions;
    class TextureImportOptions;
    class FontImportOptions;
    class AudioClipImportOptions;

    class DataStream;
    class FileStream;
    class MemoryDataStream;

    struct PHYSICS_INIT_DESC;
    class PhysicsManager;
    class Physics;
    class PhysicsScene;
    class Body;
    class RigidBody;
    class SoftBody;
    class MeshSoftBody;
    class EllipsoidSoftBody;
    class RopeSoftBody;
    class PatchSoftBody;
    class Joint;
    class HingeJoint;
    class SliderJoint;
    class SphericalJoint;
    class D6Joint;
    class ConeTwistJoint;
    class Collider;
    class BoxCollider;
    class PlaneCollider;
    class SphereCollider;
    class CylinderCollider;
    class CapsuleCollider;
    class MeshCollider;
    class ConeCollider;
    class HeightFieldCollider;
    class FCollider;
    class FJoint;
    class FBody;
    class FSoftBody;
    class PhysicsDebug;
    class PhysicsMesh;
    class FPhysicsMesh;
    class PhysicsHeightField;
    class FPhysicsHeightField;

    class GPUProfiler;

    class Picking;
    class PickingMat;
}

#include "RenderAPI/TeCommonTypes.h"
#include "Resources/TeResourceHandle.h"

namespace te
{
    typedef ResourceHandle<Resource> HResource;
    typedef ResourceHandle<Material> HMaterial;
    typedef ResourceHandle<Texture> HTexture;
    typedef ResourceHandle<Shader> HShader;
    typedef ResourceHandle<Mesh> HMesh;
    typedef ResourceHandle<ZPrepassMesh> HZPrepassMesh;
    typedef ResourceHandle<Font> HFont;
    typedef ResourceHandle<AudioClip> HAudioClip;
    typedef ResourceHandle<AnimationClip> HAnimationClip;
    typedef ResourceHandle<PhysicsMesh> HPhysicsMesh;
    typedef ResourceHandle<PhysicsHeightField> HPhysicsHeightField;
}

#include "Scene/TeGameObjectHandle.h"

namespace te
{
    typedef GameObjectHandle<SceneObject> HSceneObject;
    typedef GameObjectHandle<GameObject> HGameObject;
    typedef GameObjectHandle<CRenderable> HRenderable;
    typedef GameObjectHandle<Component> HComponent;
    typedef GameObjectHandle<CCamera> HCamera;
    typedef GameObjectHandle<CCameraFlyer> HCameraFlyer;
    typedef GameObjectHandle<CCameraUI> HCameraUI;
    typedef GameObjectHandle<CLight> HLight;
    typedef GameObjectHandle<CSkybox> HSkybox;
    typedef GameObjectHandle<CScript> HScript;
    typedef GameObjectHandle<CBone> HBone;
    typedef GameObjectHandle<CAnimation> HAnimation;
    typedef GameObjectHandle<CAudioSource> HAudioSource;
    typedef GameObjectHandle<CAudioListener> HAudioListener;
    typedef GameObjectHandle<CJoint> HJoint;
    typedef GameObjectHandle<CHingeJoint> HHingeJoint;
    typedef GameObjectHandle<CSliderJoint> HSliderJoint;
    typedef GameObjectHandle<CSphericalJoint> HSphericalJoint;
    typedef GameObjectHandle<CD6Joint> HD6Joint;
    typedef GameObjectHandle<CConeTwistJoint> HConeTwistJoint;
    typedef GameObjectHandle<CBody> HBody;
    typedef GameObjectHandle<CRigidBody> HRigidBody;
    typedef GameObjectHandle<CSoftBody> HSoftBody;
    typedef GameObjectHandle<CMeshSoftBody> HMeshSoftBody;
    typedef GameObjectHandle<CEllipsoidSoftBody> HEllipsoidSoftBody;
    typedef GameObjectHandle<CRopeSoftBody> HRopeSoftBody;
    typedef GameObjectHandle<CPatchSoftBody> HPatchSoftBody;
    typedef GameObjectHandle<CCollider> HCollider;
    typedef GameObjectHandle<CBoxCollider> HBoxCollider;
    typedef GameObjectHandle<CPlaneCollider> HPlaneCollider;
    typedef GameObjectHandle<CSphereCollider> HSphereCollider;
    typedef GameObjectHandle<CCylinderCollider> HCylinderCollider;
    typedef GameObjectHandle<CCapsuleCollider> HCapsuleCollider;
    typedef GameObjectHandle<CMeshCollider> HMeshCollider;
    typedef GameObjectHandle<CConeCollider> HConeCollider;
    typedef GameObjectHandle<CHeightFieldCollider> HHeightFieldCollider;
    typedef GameObjectHandle<CDecal> HDecal;
}
