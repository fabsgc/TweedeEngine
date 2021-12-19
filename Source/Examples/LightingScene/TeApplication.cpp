#include "TeApplication.h"

#include "Resources/TeResourceManager.h"
#include "Resources/TeBuiltinResources.h"

#include "Input/TeInput.h"
#include "Input/TeVirtualInput.h"

#include "Importer/TeImporter.h"
#include "Importer/TeTextureImportOptions.h"
#include "Importer/TeMeshImportOptions.h"

#include "Renderer/TeRenderer.h"
#include "Renderer/TeCamera.h"

#include "Scene/TeSceneManager.h"

#include "Mesh/TeMesh.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCLight.h"
#include "Components/TeCCameraFlyer.h"

#include "Utility/TeTime.h"

#include "Math/TeMath.h"

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

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;
        textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

        auto textureSkyboxImportOptions = TextureImportOptions::Create();
        textureSkyboxImportOptions->CpuCached = false;
        textureSkyboxImportOptions->CubemapType = CubemapSourceType::Faces;
        textureSkyboxImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
        textureSkyboxImportOptions->IsCubemap = true;

        _mesh = gResourceManager().Load<Mesh>("Data/Meshes/LightingScene/lighting-scene.obj", meshImportOptions);
        _skyboxTexture = gResourceManager().Load<Texture>("Data/Textures/Skybox/skybox_night_medium.png", textureSkyboxImportOptions);
        _skyboxIrradianceTexture = gResourceManager().Load<Texture>("Data/Textures/Skybox/skybox_night_irradiance_small.png", textureSkyboxImportOptions);

        HShader _shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque);

        _materials =
        {
            {
                "Floor",
                "Data/Textures/LightingScene/floor-albedo.jpeg",
                "Data/Textures/LightingScene/floor-normal.jpeg",
                "Data/Textures/LightingScene/floor-specular.jpeg"
            },
            {
                "Wall",
                "Data/Textures/LightingScene/wall-albedo.jpeg",
                "Data/Textures/LightingScene/wall-normal.jpeg",
                "Data/Textures/LightingScene/wall-specular.jpeg"
            },
            {
                "Obj",
                "Data/Textures/LightingScene/obj-albedo.jpeg",
                "Data/Textures/LightingScene/obj-normal.jpeg",
                "Data/Textures/LightingScene/obj-specular.jpeg"
            },
        };

        auto materialFunction = [&](MaterialData& material)
        {
            material.MaterialProp.UseDiffuseMap = true;
            material.MaterialProp.UseNormalMap = true;
            material.MaterialProp.UseSpecularMap = true;
            material.MaterialProp.SpecularPower = 16.0f;
            material.MaterialProp.Ambient = Color(0.05f, 0.05f, 0.05f, 0.15f);

            material.DiffuseTexture = gResourceManager().Load<Texture>(material.DiffusePath, textureImportOptions);
            material.NormalTexture = gResourceManager().Load<Texture>(material.NormalPath, textureImportOptions);
            material.SpecularTexture = gResourceManager().Load<Texture>(material.SpecularPath, textureImportOptions);

            material.MaterialElement = Material::Create(_shader);
            material.MaterialElement->SetName(material.Name);
            material.MaterialElement->SetTexture("DiffuseMap", material.DiffuseTexture);
            material.MaterialElement->SetTexture("NormalMap", material.NormalTexture);
            material.MaterialElement->SetTexture("SpecularMap", material.SpecularTexture);
            material.MaterialElement->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
            material.MaterialElement->SetProperties(material.MaterialProp);
        };

        Vector<Thread> materialThreads;
        materialThreads.reserve(3);

        for (UINT8 j = 0; j < 3; j++)
        {
            auto materialLoadFunction = std::bind(materialFunction, std::ref(_materials[j]));
            materialThreads.emplace_back(Thread(materialLoadFunction));
        }

        for (UINT8 j = 0; j < 3; j++)
        {
            materialThreads[j].join();
        }
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

        _sceneRenderableSO = SceneObject::Create("Mesh");
        _renderable = _sceneRenderableSO->AddComponent<CRenderable>();
        _renderable->SetMesh(_mesh);

        for (auto& material : _materials)
        {
            _renderable->SetMaterial(material.Name, material.MaterialElement);
        }

        _renderable->Initialize();

        _scenePointLightSO = SceneObject::Create("PointLight");
        _pointLight = _scenePointLightSO->AddComponent<CLight>(LightType::Radial);
        _pointLight->SetIntensity(5.0f);
        _pointLight->SetAttenuationRadius(1.0f);
        _pointLight->SetLinearAttenuation(0.35f);
        _pointLight->SetQuadraticAttenuation(0.44f);
        _pointLight->Initialize();

        _sceneSpotLightSO = SceneObject::Create("SpotLight");
        _spotLight = _sceneSpotLightSO->AddComponent<CLight>(LightType::Spot);
        _spotLight->SetIntensity(12.0f);
        _spotLight->SetAttenuationRadius(1.0f);
        _spotLight->SetLinearAttenuation(0.35f);
        _spotLight->SetQuadraticAttenuation(0.44f);
        _spotLight->Initialize();

        _sceneDirectionalLightSO = SceneObject::Create("DirectionalLight");
        _directionalLight = _sceneDirectionalLightSO->AddComponent<CLight>(LightType::Directional);
        _directionalLight->SetIntensity(0.1f);
        _directionalLight->Initialize();

        _sceneSkyboxSO = SceneObject::Create("Skybox");
        _skybox = _sceneSkyboxSO->AddComponent<CSkybox>();
        _skybox->SetTexture(_skyboxTexture);
        _skybox->SetIrradiance(_skyboxIrradianceTexture);
        _skybox->SetBrightness(0.5f);
        _skybox->Initialize();

        _sceneCameraSO->SetPosition(Vector3(7.5f, 5.0f, 7.5f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 0.0f, 0.0f));

        _scenePointLightSO->SetPosition(Vector3(2.0f, 2.0f, 0.0f));
        _sceneSpotLightSO->SetPosition(Vector3(3.0f, 3.0f, 3.0f));
        _sceneSpotLightSO->Rotate(Vector3(0.0f, 1.0f, 1.0f), Radian(Math::HALF_PI / 2.0f));
        _sceneDirectionalLightSO->SetPosition(Vector3(3.0f, 10.0f, 10.0f));
        _sceneDirectionalLightSO->Rotate(Vector3(0.0f, 1.0f, 1.0f), -Radian(Math::HALF_PI));

        auto settings = _sceneCamera->GetRenderSettings();
        settings->ExposureScale = 1.0f;
        settings->Gamma = 1.0f;
        // ######################################################

        // ######################################################
        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
        // ######################################################
#endif
    }

    void Application::PreShutDown()
    { }

    void Application::PreUpdate()
    { }

    void Application::PostUpdate()
    { }
}
