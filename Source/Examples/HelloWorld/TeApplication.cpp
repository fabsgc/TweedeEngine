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
#include "Components/TeCSkybox.h"
#include "Components/TeCCameraFlyer.h"

#include "Material/TeMaterial.h"
#include "Material/TeShader.h"

#include "Utility/TeTime.h"

#include "Profiling/TeProfilerGPU.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Application)

    void Application::PostStartUp()
    {
        gProfilerGPU().Enable(false);

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

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;
        textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

        auto textureCubeMapImportOptions = TextureImportOptions::Create();
        textureCubeMapImportOptions->CpuCached = false;
        textureCubeMapImportOptions->CubemapType = CubemapSourceType::Faces;
        textureCubeMapImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
        textureCubeMapImportOptions->IsCubemap = true;

        _loadedMeshMonkey = gResourceManager().Load<Mesh>("Data/Meshes/Monkey/monkey.obj", meshImportOptions);
        _loadedTextureMonkey = gResourceManager().Load<Texture>("Data/Textures/Monkey/diffuse.png", textureImportOptions);
        _loadedCubemapTexture = gResourceManager().Load<Texture>("Data/Textures/Skybox/skybox_day_medium.png", textureCubeMapImportOptions);
        _loadedCubemapIrradianceTexture = gResourceManager().Load<Texture>("Data/Textures/Skybox/skybox_day_irradiance_small.png", textureCubeMapImportOptions);

        TE_PRINT((_loadedMeshMonkey.GetHandleData())->data);
        TE_PRINT((_loadedMeshMonkey.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedTextureMonkey.GetHandleData())->data);
        TE_PRINT((_loadedTextureMonkey.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedCubemapTexture.GetHandleData())->data);
        TE_PRINT((_loadedCubemapTexture.GetHandleData())->uuid.ToString());
        // ######################################################

        // ######################################################
        HShader _shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque);

        MaterialProperties properties;
        properties.Ambient = Color(0.05f, 0.05f, 0.05f, 0.4f);
        properties.UseDiffuseMap = true;

        _materialMonkey = Material::Create(_shader);
        _materialMonkey->SetName("Material");
        _materialMonkey->SetTexture("DiffuseMap", _loadedTextureMonkey);
        _materialMonkey->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
        _materialMonkey->SetProperties(properties);
        UINT32 value = 1;
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

        _sceneSkyboxSO = SceneObject::Create("Skybox");
        _skybox = _sceneSkyboxSO->AddComponent<CSkybox>();
        _skybox->SetTexture(_loadedCubemapTexture);
        _skybox->SetIrradiance(_loadedCubemapIrradianceTexture);
        _skybox->Initialize();

        _sceneLightSO = SceneObject::Create("Light");
        _light = _sceneLightSO->AddComponent<CLight>();
        _light->Initialize();

        _sceneCameraSO->SetPosition(Vector3(0.0f, 5.0f, 7.5f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 0.0f, -3.0f));

        for (INT16 i = -15; i < 16; i++)
        {
            for (INT16 j = -1; j < 16; j++)
            {
                HSceneObject sceneRenderable = SceneObject::Create("Monkey_" + ToString(i) + "_" + ToString(j));
                HRenderable renderable = sceneRenderable->AddComponent<CRenderable>();
                renderable->SetMesh(_loadedMeshMonkey);
                renderable->SetMaterial(_materialMonkey);
                renderable->SetInstancing(true);
                renderable->SetWriteVelocity(true);
                renderable->Initialize();

                sceneRenderable->Move(Vector3((float)i * 3.0f, 0.0f, -(float)j * 3.0f));

                _sceneRenderablesMonkeySO.push_back(sceneRenderable);
            }
        }

        auto settings = _sceneCamera->GetRenderSettings();
        settings->ExposureScale = 1.3f;
        settings->Gamma = 1.0f;
        settings->Contrast = 1.60f;
        settings->Brightness = -0.05f;
        settings->Bloom.Enabled = false;
        settings->MotionBlur.Enabled = false;
        settings->MotionBlur.Quality = MotionBlurQuality::High;
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
        for (auto& so : _sceneRenderablesMonkeySO)
        {
            so->Rotate(Vector3(0.0f, 1.0f, 0.0f), Radian(2.5f * gTime().GetFrameDelta()));
        }
#endif
    }

    void Application::PostUpdate()
    { }
}
