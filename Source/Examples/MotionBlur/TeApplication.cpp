#include "TeApplication.h"

#include "Resources/TeResourceManager.h"
#include "Resources/TeBuiltinResources.h"

#include "Input/TeInput.h"
#include "Input/TeVirtualInput.h"

#include "Importer/TeImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"

#include "Renderer/TeRenderer.h"
#include "Renderer/TeCamera.h"

#include "Scene/TeSceneManager.h"

#include "Mesh/TeMesh.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCCameraFlyer.h"

#include "Material/TeMaterial.h"
#include "Material/TeShader.h"

#include "Utility/TeTime.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Application)

    void Application::PostStartUp()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        // ######################################################
        auto inputConfig = gVirtualInput().GetConfiguration();

        inputConfig->RegisterButton("Forward", TE_W);
        inputConfig->RegisterButton("Back", TE_S);
        inputConfig->RegisterButton("Left", TE_A);
        inputConfig->RegisterButton("Right", TE_D);
        inputConfig->RegisterButton("Forward", TE_UP);
        inputConfig->RegisterButton("Back", TE_DOWN);
        inputConfig->RegisterButton("Left", TE_LEFT);
        inputConfig->RegisterButton("Right", TE_RIGHT);
        inputConfig->RegisterButton("FastMove", TE_LSHIFT);
        inputConfig->RegisterButton("RotateObj", TE_MOUSE_LEFT);
        inputConfig->RegisterButton("RotateCam", TE_MOUSE_RIGHT);

        inputConfig->RegisterAxis("Horizontal", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
        inputConfig->RegisterAxis("Vertical", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
        // ######################################################

        // ######################################################
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = true;
        meshImportOptions->ImportTangents = true;
        meshImportOptions->CpuCached = false;

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;

        auto textureCubeMapImportOptions = TextureImportOptions::Create();
        textureCubeMapImportOptions->CpuCached = false;
        textureCubeMapImportOptions->CubemapType = CubemapSourceType::Faces;
        textureCubeMapImportOptions->Format = PF_RGBA8;
        textureCubeMapImportOptions->IsCubemap = true;

        _loadedMeshCube = gResourceManager().Load<Mesh>("Data/Meshes/Cube/cube.obj", meshImportOptions);
        _loadedMeshMill = gResourceManager().Load<Mesh>("Data/Meshes/Mill/mill.obj", meshImportOptions);
        _loadedTextureCube = gResourceManager().Load<Texture>("Data/Textures/Cube/diffuse.png", textureImportOptions);

        TE_PRINT((_loadedMeshCube.GetHandleData())->data);
        TE_PRINT((_loadedMeshCube.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedMeshMill.GetHandleData())->data);
        TE_PRINT((_loadedMeshMill.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedTextureCube.GetHandleData())->data);
        TE_PRINT((_loadedTextureCube.GetHandleData())->uuid.ToString());
        // ######################################################

        // ######################################################
        HShader _shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque);

        MaterialProperties properties;
        properties.UseDiffuseMap = true;

        _materialCube = Material::Create(_shader);
        _materialCube->SetName("MaterialCube");
        _materialCube->SetTexture("DiffuseMap", _loadedTextureCube);
        _materialCube->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
        _materialCube->SetProperties(properties);

        properties.UseDiffuseMap = false;
        properties.Ambient = Color(0.142f, 0.05f, 0.015f);

        _materialMill = Material::Create(_shader);
        _materialMill->SetName("MaterialMill");
        _materialMill->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
        _materialMill->SetProperties(properties);
        // ######################################################

        // ######################################################
        _sceneCameraSO = SceneObject::Create("SceneCamera");
        _sceneCameraFlyer = _sceneCameraSO->AddComponent<CCameraFlyer>();
        _sceneCamera = _sceneCameraSO->AddComponent<CCamera>();
        _sceneCamera->GetViewport()->SetClearColorValue(Color(0.17f, 0.64f, 1.0f, 1.0f));
        _sceneCamera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        _sceneCamera->SetMSAACount(_window->GetDesc().MultisampleCount);
        _sceneCamera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        _sceneCamera->SetMain(true);
        _sceneCamera->Initialize();

        _sceneCubeSO = SceneObject::Create("Cube");
        _renderableCube = _sceneCubeSO->AddComponent<CRenderable>();
        _renderableCube->SetMesh(_loadedMeshCube);
        _renderableCube->SetMaterial(_materialCube);
        _renderableCube->Initialize();

        _sceneMillSO = SceneObject::Create("Mill");
        _renderableMill = _sceneMillSO->AddComponent<CRenderable>();
        _renderableMill->SetMesh(_loadedMeshMill);
        _renderableMill->SetMaterial(_materialMill);
        _renderableMill->Initialize();

        _sceneLightSO = SceneObject::Create("Light");
        _light = _sceneLightSO->AddComponent<CLight>(LightType::Directional);
        _light->SetIntensity(0.075f);
        _light->Initialize();

        _sceneCameraSO->SetPosition(Vector3(0.0f, 0.0f, 5.0f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 0.0f, -5.0f));

        _sceneCubeSO->Move(Vector3(-10.0f, -5.0f, -10.0f));
        _sceneMillSO->Move(Vector3(0.0f, 0.0f, -12.0f));
        _sceneLightSO->Rotate(Vector3(0.0f, 1.0f, 1.0f), -Radian(Math::HALF_PI));

        auto settings = _sceneCamera->GetRenderSettings();
        settings->ExposureScale = 1.3f;
        settings->Gamma = 1.0f;
        settings->Contrast = 1.60f;
        settings->Brightness = -0.05f;
        // ######################################################

        // ######################################################
        gRenderer()->BatchRenderables();
        // ######################################################

        // ######################################################
        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
        // ######################################################
#endif
    }

    void Application::PreShutDown()
    { }

    void Application::PreUpdate()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        Vector3 position = _sceneCubeSO->GetTransform().GetPosition();

        if(position.x > 10.0f)
        {
            _direction = CubeDirection::GoRight;
        }
        if(position.x < -10.0f)
        {
            _direction = CubeDirection::GoLeft;
        }

        if(_direction == CubeDirection::GoLeft)
        {
            _sceneCubeSO->Move(Vector3(7.5f * gTime().GetFrameDelta(), 0.0f, 0.0f));
        }
        else
        {
            _sceneCubeSO->Move(Vector3(-7.5f * gTime().GetFrameDelta(), 0.0f, 0.0f));
        }

        _sceneMillSO->Rotate(Vector3(0.0f, 0.0f, 1.0f), Radian(2.5f * gTime().GetFrameDelta()));
#endif
    }

    void Application::PostUpdate()
    { }
}
