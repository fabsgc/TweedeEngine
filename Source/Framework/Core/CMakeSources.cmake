set (TE_CORE_INC_RENDERAPI
    "Core/RenderAPI/TeRenderAPI.h"
    "Core/RenderAPI/TeRenderWindow.h"
    "Core/RenderAPI/TeVideoMode.h"
    "Core/RenderAPI/TeRenderAPIFactory.h"
    "Core/RenderAPI/TeRenderTarget.h"
    "Core/RenderAPI/TeVertexBuffer.h"
    "Core/RenderAPI/TeIndexBuffer.h"
    "Core/RenderAPI/TeVertexDeclaration.h"
    "Core/RenderAPI/TeCommonTypes.h"
    "Core/RenderAPI/TeDepthStencilState.h"
    "Core/RenderAPI/TeGpuPipelineState.h"
    "Core/RenderAPI/TeRasterizerState.h"
    "Core/RenderAPI/TeRenderStateManager.h"
    "Core/RenderAPI/TeBlendState.h"
    "Core/RenderAPI/TeSamplerState.h"
    "Core/RenderAPI/TeVertexDataDesc.h"
    "Core/RenderAPI/TeSubMesh.h"
    "Core/RenderAPI/TeTextureView.h"
    "Core/RenderAPI/TeHardwareBuffer.h"
    "Core/RenderAPI/TeRenderTexture.h"
    "Core/RenderAPI/TeGpuProgram.h"
    "Core/RenderAPI/TeGpuBuffer.h"
    "Core/RenderAPI/TeGpuParamDesc.h"
    "Core/RenderAPI/TeGpuProgramManager.h"
    "Core/RenderAPI/TeHardwareBufferManager.h"
    "Core/RenderAPI/TeGpuPipelineParamInfo.h"
    "Core/RenderAPI/TeGpuParams.h"
    "Core/RenderAPI/TeGpuParam.h"
    "Core/RenderAPI/TeGpuParamBlockBuffer.h"
    "Core/RenderAPI/TeVertexData.h"
)
set (TE_CORE_SRC_RENDERAPI
    "Core/RenderAPI/TeRenderAPI.cpp"
    "Core/RenderAPI/TeRenderWindow.cpp"
    "Core/RenderAPI/TeVideoMode.cpp"
    "Core/RenderAPI/TeRenderTarget.cpp"
    "Core/RenderAPI/TeVertexBuffer.cpp"
    "Core/RenderAPI/TeIndexBuffer.cpp"
    "Core/RenderAPI/TeVertexDeclaration.cpp"
    "Core/RenderAPI/TeDepthStencilState.cpp"
    "Core/RenderAPI/TeGpuPipelineState.cpp"
    "Core/RenderAPI/TeRasterizerState.cpp"
    "Core/RenderAPI/TeRenderStateManager.cpp"
    "Core/RenderAPI/TeBlendState.cpp"
    "Core/RenderAPI/TeSamplerState.cpp"
    "Core/RenderAPI/TeVertexDataDesc.cpp"
    "Core/RenderAPI/TeTextureView.cpp"
    "Core/RenderAPI/TeRenderTexture.cpp"
    "Core/RenderAPI/TeGpuProgram.cpp"
    "Core/RenderAPI/TeGpuBuffer.cpp"
    "Core/RenderAPI/TeGpuProgramManager.cpp"
    "Core/RenderAPI/TeHardwareBufferManager.cpp"
    "Core/RenderAPI/TeGpuPipelineParamInfo.cpp"
    "Core/RenderAPI/TeGpuParams.cpp"
    "Core/RenderAPI/TeGpuParam.cpp"
    "Core/RenderAPI/TeGpuParamBlockBuffer.cpp"
    "Core/RenderAPI/TeVertexData.cpp"
)

set (TE_CORE_INC_RENDERER
    "Core/Renderer/TeRenderer.h"
    "Core/Renderer/TeRendererFactory.h"
    "Core/Renderer/TeCamera.h"
    "Core/Renderer/TeViewport.h"
    "Core/Renderer/TeRenderSettings.h"
    "Core/Renderer/TeRendererMeshData.h"
    "Core/Renderer/TeParamBlocks.h"
    "Core/Renderer/TeRenderable.h"
    "Core/Renderer/TeLight.h"
    "Core/Renderer/TeRenderQueue.h"
    "Core/Renderer/TeRenderElement.h"
    "Core/Renderer/TeSkybox.h"
    "Core/Renderer/TeRendererUtility.h"
    "Core/Renderer/TeGpuResourcePool.h"
)
set (TE_CORE_SRC_RENDERER
    "Core/Renderer/TeRenderer.cpp"
    "Core/Renderer/TeCamera.cpp"
    "Core/Renderer/TeViewport.cpp"
    "Core/Renderer/TeRendererMeshData.cpp"
    "Core/Renderer/TeParamBlocks.cpp"
    "Core/Renderer/TeRenderable.cpp"
    "Core/Renderer/TeLight.cpp"
    "Core/Renderer/TeRenderQueue.cpp"
    "Core/Renderer/TeRenderElement.cpp"
    "Core/Renderer/TeSkybox.cpp"
    "Core/Renderer/TeRendererUtility.cpp"
    "Core/Renderer/TeGpuResourcePool.cpp"
)

set (TE_CORE_INC_SCENE
    "Core/Scene/TeSceneActor.h"
    "Core/Scene/TeSceneManager.h"
    "Core/Scene/TeTransform.h"
    "Core/Scene/TeComponent.h"
    "Core/Scene/TeGameObject.h"
    "Core/Scene/TeGameObjectHandle.h"
    "Core/Scene/TeGameObjectManager.h"
    "Core/Scene/TeSceneObject.h"
)
set (TE_CORE_SRC_SCENE
    "Core/Scene/TeSceneActor.cpp"
    "Core/Scene/TeSceneManager.cpp"
    "Core/Scene/TeTransform.cpp"
    "Core/Scene/TeComponent.cpp"
    "Core/Scene/TeGameObject.cpp"
    "Core/Scene/TeGameObjectHandle.cpp"
    "Core/Scene/TeGameObjectManager.cpp"
    "Core/Scene/TeSceneObject.cpp"
)

set(TE_CORE_INC_PLATFORM
    "Core/Platform/TePlatform.h"
)
set(TE_CORE_SRC_PLATFORM 
)

set(TE_CORE_INC_CORE_UTILITY
    "Core/CoreUtility/TeCoreObject.h"
    "Core/CoreUtility/TeCoreObjectManager.h"
)
set(TE_CORE_SRC_CORE_UTILITY
    "Core/CoreUtility/TeCoreObject.cpp"
    "Core/CoreUtility/TeCoreObjectManager.cpp"
)

set (TE_CORE_INC_PLATFORM_WIN32
    "Core/Private/Win32/TeWin32Window.h"
    "Core/Private/Win32/TeWin32Input.h"
    "Core/Private/Win32/TeWin32Platform.h"
)
set (TE_CORE_SRC_PLATFORM_WIN32
    "Core/Private/Win32/TeWin32Window.cpp"
    "Core/Private/Win32/TeWin32Input.cpp"
    "Core/Private/Win32/TeWin32GamePad.cpp"
    "Core/Private/Win32/TeWin32Keyboard.cpp"
    "Core/Private/Win32/TeWin32Mouse.cpp"
    "Core/Private/Win32/TeWin32Platform.cpp"
)

set (TE_CORE_INC_PLATFORM_LINUX
    "Core/Private/Linux/TeLinuxWindow.h"
    "Core/Private/Linux/TeLinuxInput.h"
    "Core/Private/Linux/TeLinuxPlatform.h"
)
set (TE_CORE_SRC_PLATFORM_LINUX
    "Core/Private/Linux/TeLinuxWindow.cpp"
    "Core/Private/Linux/TeLinuxInput.cpp"
    "Core/Private/Linux/TeLinuxGamePad.cpp"
    "Core/Private/Linux/TeLinuxKeyboard.cpp"
    "Core/Private/Linux/TeLinuxMouse.cpp"
    "Core/Private/Linux/TeLinuxPlatform.cpp"
)

set(TE_CORE_INC_NOFILTER
    "Core/TeCoreApplication.h"
    "Core/TeCorePrerequisites.h"
    "Core/TeEngineConfig.h"
)
set(TE_CORE_SRC_NOFILTER
    "Core/TeCoreApplication.cpp"
)

set (TE_CORE_INC_INPUT
    "Core/Input/TeInput.h"
    "Core/Input/TeGamePad.h"
    "Core/Input/TeKeyboard.h"
    "Core/Input/TeMouse.h"
    "Core/Input/TeInputData.h"
    "Core/Input/TeVirtualInput.h"
    "Core/Input/TeInputConfiguration.h"
)
set (TE_CORE_SRC_INPUT
    "Core/Input/TeInput.cpp"
    "Core/Input/TeVirtualInput.cpp"
    "Core/Input/TeInputConfiguration.cpp"
)

set (TE_CORE_INC_MANAGER
    "Core/Manager/TeRenderAPIManager.h"
    "Core/Manager/TeRendererManager.h"
    "Core/Manager/TePluginManager.h"
)
set (TE_CORE_SRC_MANAGER
    "Core/Manager/TeRenderAPIManager.cpp"
    "Core/Manager/TeRendererManager.cpp"
)

set (TE_CORE_INC_IMPORTER
    "Core/Importer/TeImporter.h"
    "Core/Importer/TeImportOptions.h"
    "Core/Importer/TeBaseImporter.h"
    "Core/Importer/TeTextureImportOptions.h"
    "Core/Importer/TeMeshImportOptions.h"
    "Core/Importer/TeShaderImportOptions.h"
)
set (TE_CORE_SRC_IMPORTER
    "Core/Importer/TeImporter.cpp"
    "Core/Importer/TeImportOptions.cpp"
    "Core/Importer/TeBaseImporter.cpp"
    "Core/Importer/TeTextureImportOptions.cpp"
    "Core/Importer/TeMeshImportOptions.cpp"
    "Core/Importer/TeShaderImportOptions.cpp"
)

set (TE_CORE_INC_IMAGE
    "Core/Image/TeTexture.h"
    "Core/Image/TeTextureManager.h"
    "Core/Image/TePixelData.h"
    "Core/Image/TePixelUtil.h"
    "Core/Image/TePixelVolume.h"
    "Core/Image/TeColor.h"
)
set (TE_CORE_SRC_IMAGE
    "Core/Image/TeTexture.cpp"
    "Core/Image/TeTextureManager.cpp"
    "Core/Image/TePixelData.cpp"
    "Core/Image/TePixelUtil.cpp"
    "Core/Image/TeColor.cpp"
)

set (TE_CORE_INC_MESH
    "Core/Mesh/TeMesh.h"
    "Core/Mesh/TeMeshData.h"
    "Core/Mesh/TeMeshUtility.h"
)
set (TE_CORE_SRC_MESH
    "Core/Mesh/TeMesh.cpp"
    "Core/Mesh/TeMeshData.cpp"
    "Core/Mesh/TeMeshUtility.cpp"
)

set (TE_CORE_INC_MATERIAL
    "Core/Material/TeShader.h"
    "Core/Material/TeMaterial.h"
    "Core/Material/TeTechnique.h"
    "Core/Material/TePass.h"
)
set (TE_CORE_SRC_MATERIAL
    "Core/Material/TeShader.cpp"
    "Core/Material/TeMaterial.cpp"
    "Core/Material/TeTechnique.cpp"
    "Core/Material/TePass.cpp"
)

set (TE_CORE_INC_SERIALIZATION
    "Core/Serialization/TeSerializable.h"
)
set (TE_CORE_SRC_SERIALIZATION
)

set (TE_CORE_INC_COMPONENTS
    "Core/Components/TeCCamera.h"
    "Core/Components/TeCRenderable.h"
    "Core/Components/TeCLight.h"
    "Core/Components/TeCSkybox.h"
    "Core/Components/TeCCameraFlyer.h"
)
set (TE_CORE_SRC_COMPONENTS
    "Core/Components/TeCCamera.cpp"
    "Core/Components/TeCRenderable.cpp"
    "Core/Components/TeCLight.cpp"
    "Core/Components/TeCSkybox.cpp"
    "Core/Components/TeCCameraFlyer.cpp"
)

set (TE_CORE_INC_RESOURCE
    "Core/Resources/TeResource.h"
    "Core/Resources/TeResourceManager.h"
    "Core/Resources/TeResourceHandle.h"
    "Core/Resources/TeGpuResourceData.h"
    "Core/Resources/TeBuiltinResources.h"
)
set (TE_CORE_SRC_RESOURCE
    "Core/Resources/TeResource.cpp"
    "Core/Resources/TeResourceManager.cpp"
    "Core/Resources/TeResourceHandle.cpp"
    "Core/Resources/TeGpuResourceData.cpp"
    "Core/Resources/TeBuiltinResources.cpp"
)

if (WIN32)
    list(APPEND TE_CORE_INC_PLATFORM ${TE_CORE_INC_PLATFORM_WIN32})
    list(APPEND TE_CORE_SRC_PLATFORM ${TE_CORE_SRC_PLATFORM_WIN32})
elseif (LINUX)
    list(APPEND TE_CORE_INC_PLATFORM ${TE_CORE_INC_PLATFORM_LINUX})
    list(APPEND TE_CORE_SRC_PLATFORM ${TE_CORE_SRC_PLATFORM_LINUX})
endif ()

list(APPEND TE_CORE_INC_PLATFORM ${TE_CORE_INC_PLATFORM})
list(APPEND TE_CORE_SRC_PLATFORM ${TE_CORE_SRC_PLATFORM})

source_group("Core\\RenderAPI" FILES ${TE_CORE_INC_RENDERAPI} ${TE_CORE_SRC_RENDERAPI})
source_group("Core\\Renderer" FILES ${TE_CORE_INC_RENDERER} ${TE_CORE_SRC_RENDERER})
source_group("Core\\Platform" FILES ${TE_CORE_INC_PLATFORM} ${TE_CORE_SRC_PLATFORM})
source_group("Core\\Input" FILES ${TE_CORE_INC_INPUT} ${TE_CORE_SRC_INPUT})
source_group("Core\\Manager" FILES ${TE_CORE_INC_MANAGER} ${TE_CORE_SRC_MANAGER})
source_group("Core\\Importer" FILES ${TE_CORE_INC_IMPORTER} ${TE_CORE_SRC_IMPORTER})
source_group("Core\\Image" FILES ${TE_CORE_INC_IMAGE} ${TE_CORE_SRC_IMAGE})
source_group("Core\\Mesh" FILES ${TE_CORE_INC_MESH} ${TE_CORE_SRC_MESH})
source_group("Core\\Resources" FILES ${TE_CORE_INC_RESOURCE} ${TE_CORE_SRC_RESOURCE})
source_group("Core\\Scene" FILES ${TE_CORE_INC_SCENE} ${TE_CORE_SRC_SCENE})
source_group("Core\\Material" FILES ${TE_CORE_INC_MATERIAL} ${TE_CORE_SRC_MATERIAL})
source_group("Core\\CoreUtility" FILES ${TE_CORE_INC_CORE_UTILITY} ${TE_CORE_SRC_CORE_UTILITY})
source_group("Core\\Serialization" FILES ${TE_CORE_INC_SERIALIZATION} ${TE_CORE_SRC_SERIALIZATION})
source_group("Core\\Components" FILES ${TE_CORE_INC_COMPONENTS} ${TE_CORE_SRC_COMPONENTS})
source_group("Core" FILES ${TE_CORE_INC_NOFILTER} ${TE_CORE_SRC_NOFILTER})

set(TE_CORE_SRC
    ${TE_CORE_SRC_RENDERAPI}
    ${TE_CORE_INC_RENDERAPI}
    ${TE_CORE_SRC_RENDERER}
    ${TE_CORE_INC_RENDERER}
    ${TE_CORE_SRC_PLATFORM}
    ${TE_CORE_INC_PLATFORM}
    ${TE_CORE_SRC_NOFILTER}
    ${TE_CORE_INC_NOFILTER}
    ${TE_CORE_INC_INPUT}
    ${TE_CORE_SRC_INPUT}
    ${TE_CORE_INC_MANAGER}
    ${TE_CORE_SRC_MANAGER}
    ${TE_CORE_INC_IMPORTER}
    ${TE_CORE_SRC_IMPORTER}
    ${TE_CORE_INC_IMAGE}
    ${TE_CORE_SRC_IMAGE}
    ${TE_CORE_INC_MESH}
    ${TE_CORE_SRC_MESH}
    ${TE_CORE_INC_RESOURCE}
    ${TE_CORE_SRC_RESOURCE}
    ${TE_CORE_INC_SCENE}
    ${TE_CORE_SRC_SCENE}
    ${TE_CORE_INC_MATERIAL}
    ${TE_CORE_SRC_MATERIAL}
    ${TE_CORE_INC_CORE_UTILITY}
    ${TE_CORE_SRC_CORE_UTILITY}
    ${TE_CORE_INC_SERIALIZATION}
    ${TE_CORE_SRC_SERIALIZATION}
    ${TE_CORE_INC_COMPONENTS}
    ${TE_CORE_SRC_COMPONENTS}
)
