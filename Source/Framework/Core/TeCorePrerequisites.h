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

namespace te
{
    enum class CoreType
    {
        TID_Unknown                   = 1000,

        TID_SceneObject               = 1001,

        TID_Component                 = 1100,
        
        TID_CLight                    = 1101,
        TID_CDecal                    = 1102,
        TID_CSkybox                   = 1103,
        TID_CRenderable               = 1104,

        TID_CCamera                   = 1105,
        TID_CCameraUI                 = 1106,
        TID_CCameraFlyer              = 1107,
        
        TID_CBone                     = 1108,
        TID_CAnimation                = 1109,

        TID_CAudioSource              = 1110,
        TID_CAudioListener            = 1111,
        
        TID_CBody                     = 1112,
        TID_CSoftBody                 = 1113,
        TID_CRigidBody                = 1114,
        TID_CRopeSoftBody             = 1115,
        TID_CPatchSoftBody            = 1116,

        TID_CJoint                    = 1117,
        TID_CD6Joint                  = 1118,
        TID_CHingeJoint               = 1119,
        TID_CSliderJoint              = 1120,
        TID_CSphericalJoint           = 1121,
        TID_CConeTwistJoint           = 1122,
        
        TID_CCollider                 = 1123,
        TID_CBoxCollider              = 1124,
        TID_CMeshCollider             = 1125,
        TID_CConeCollider             = 1126,
        TID_CPlaneCollider            = 1127,
        TID_CSphereCollider           = 1128,
        TID_CCylinderCollider         = 1129,
        TID_CCapsuleCollider          = 1130,
        TID_CHeightFieldCollider      = 1131,

        TID_Light                     = 1200,
        TID_Decal                     = 1201,
        TID_Skybox                    = 1202,
        TID_Camera                    = 1203,
        TID_Renderable                = 1204,

        TID_Skeleton                  = 1300,
        
        TID_Resource                  = 1400,
        TID_Project                   = 1401,
        TID_Script                    = 1402,
        TID_Font                      = 1403,
        TID_AudioClip                 = 1404,
        TID_AnimationClip             = 1405,
        TID_Animation                 = 1406,
        TID_Texture                   = 1407,
        TID_Material                  = 1408,
        TID_Pass                      = 1409,
        TID_Shader                    = 1410,
        TID_Technique                 = 1411,

        TID_ZPrepassMesh              = 1412,
        TID_Mesh                      = 1413,

        TID_FSoftBody                 = 1500,
        TID_FPhysicsMesh              = 1501,
        TID_FPhysicsHeightField       = 1502,
        TID_FJoint                    = 1503,
        TID_FCollider                 = 1504,
        TID_FBody                     = 1505,
        
        TID_PhysicsHeightField        = 1506,
        TID_PhysicsMesh               = 1507,

        TID_RigidBody                 = 1508,
        TID_SoftBody                  = 1509,
        TID_MeshSoftBody              = 1510,
        TID_RopeSoftBody              = 1511,
        TID_PatchSoftBody             = 1512,
        TID_CMeshSoftBody             = 1513,
        TID_EllipsoidSoftBody         = 1514,
        TID_CEllipsoidSoftBody        = 1515,
        TID_D6Joint                   = 1516,
        TID_HingeJoint                = 1517,
        TID_SliderJoint               = 1518,
        TID_SphericalJoint            = 1519,
        TID_ConeTwistJoint            = 1520,
        TID_BoxCollider               = 1521,
        TID_PlaneCollider             = 1522,
        TID_SphereCollider            = 1523,
        TID_CylinderCollider          = 1524,
        TID_CapsuleCollider           = 1525,
        TID_MeshCollider              = 1526,
        TID_ConeCollider              = 1527,
        TID_HeightFieldCollider       = 1528,
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
    class ResourceListener;
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
    class StreamWriter;
    class StreamReader;
    class BinaryWriter;
    class BinaryReader;

    class CCamera;
    class CCameraFlyer;
    class CCameraUI;
    class CRenderable;
    class CLight;
    class CSkybox;
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
    class FontImportOptions;
    class TextureImportOptions;
    class MeshImportOptions;
    class ShaderImportOptions;
    class AudioClipImportOptions;

    class ProjectImportOptions;
    class ResourceImportOptions;

    class Exporter;
    class BaseExporter;
    class ExportOptions;
    class ProjectExportOptions;
    class ResourceExportOptions;
    class TextureExportOptions;

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

    class Project;
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
    typedef ResourceHandle<Project> HProject;
    typedef ResourceHandle<Script> HScript;
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
