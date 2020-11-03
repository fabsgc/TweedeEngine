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
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCLight.h"

#include "Material/TeMaterial.h"
#include "Material/TeShader.h"

#include "Utility/TeTime.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Application)

    void Application::InitInputHandling()
    {
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
    }

    void Application::InitShader()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        _shaderOpaque = gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque);
        _shaderTransparent = gBuiltinResources().GetBuiltinShader(BuiltinShader::Transparent);
#endif
    }

    void Application::InitMaterials()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        auto textureCubeMapImportOptions = TextureImportOptions::Create();
        textureCubeMapImportOptions->CpuCached = false;
        textureCubeMapImportOptions->CubemapType = CubemapSourceType::Faces;
        textureCubeMapImportOptions->Format = PF_RGBA8;
        textureCubeMapImportOptions->IsCubemap = true;

        _loadedSkyboxTexture = gResourceManager().Load<Texture>("Data/Textures/Skybox/sky_night_2_large.jpeg", textureCubeMapImportOptions);

        _materials =
          {
            {
                "arch",
                "Data/Textures/Sponza/arch/arch_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/arch/arch_NRM.jpeg",
                "Data/Textures/Sponza/arch/arch_SPEC.jpeg",
                "",
                "",
                "",
                "Data/Textures/Sponza/arch/arch_OCC.jpeg",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "background",
                "Data/Textures/Sponza/background/background_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/background/background_NRM.jpeg",
                "Data/Textures/Sponza/background/background_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "bricks",
                "Data/Textures/Sponza/bricks/bricks_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/bricks/bricks_NRM_3.jpeg",
                "Data/Textures/Sponza/bricks/bricks_SPEC.jpeg",
                "",
                "",
                "",
                "Data/Textures/Sponza/bricks/bricks_OCC.jpeg",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "ceiling",
                "Data/Textures/Sponza/ceiling/ceiling_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/ceiling/ceiling_NRM.jpeg",
                "Data/Textures/Sponza/ceiling/ceiling_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "chain",
                "Data/Textures/Sponza/chain/chain_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/chain/chain_NRM.jpeg",
                "Data/Textures/Sponza/chain/chain_SPEC.jpeg",
                "",
                "",
                "Data/Textures/Sponza/chain/chain_MASK.jpeg",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "column_a",
                "Data/Textures/Sponza/column_a/column_a_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/column_a/column_a_NRM.jpeg",
                "Data/Textures/Sponza/column_a/column_a_SPEC.jpeg",
                "",
                "",
                "",
                "Data/Textures/Sponza/column_a/column_a_OCC.jpeg",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "column_b",
                "Data/Textures/Sponza/column_b/column_b_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/column_b/column_b_NRM.jpeg",
                "Data/Textures/Sponza/column_b/column_b_SPEC.jpeg",
                "",
                "",
                "",
                "Data/Textures/Sponza/column_b/column_b_OCC.jpeg",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "column_c",
                "Data/Textures/Sponza/column_c/column_c_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/column_c/column_c_NRM.jpeg",
                "Data/Textures/Sponza/column_c/column_c_SPEC.jpeg",
                "",
                "",
                "",
                "Data/Textures/Sponza/column_c/column_c_OCC.jpeg",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "details",
                "Data/Textures/Sponza/details/details_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/details/details_NRM.jpeg",
                "Data/Textures/Sponza/details/details_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "fabric_a",
                "Data/Textures/Sponza/fabric_a/fabric_a_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/fabric_a/fabric_a_NRM.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "fabric_c",
                "Data/Textures/Sponza/fabric_c/fabric_c_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/fabric_c/fabric_c_NRM.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "fabric_d",
                "Data/Textures/Sponza/fabric_d/fabric_d_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/fabric_a/fabric_a_NRM.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "fabric_e",
                "Data/Textures/Sponza/fabric_e/fabric_e_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/fabric_a/fabric_a_NRM.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "fabric_f",
                "Data/Textures/Sponza/fabric_f/fabric_f_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/fabric_c/fabric_c_NRM.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "fabric_g",
                "Data/Textures/Sponza/fabric_g/fabric_g_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/fabric_c/fabric_c_NRM.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "flagpole",
                "Data/Textures/Sponza/flagpole/flagpole_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/flagpole/flagpole_NRM.jpeg",
                "Data/Textures/Sponza/flagpole/flagpole_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "floor",
                "Data/Textures/Sponza/floor/floor_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/floor/floor_NRM_2.jpeg",
                "Data/Textures/Sponza/floor/floor_SPEC.jpeg",
                "",
                "",
                "",
                "Data/Textures/Sponza/floor/floor_OCC.jpeg",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "leaf",
                "Data/Textures/Sponza/leaf/leaf_COLOR.jpeg",
                "",
                "",
                "Data/Textures/Sponza/leaf/leaf_SPEC.jpeg",
                "",
                "",
                "Data/Textures/Sponza/leaf/leaf_MASK.jpeg",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "lion",
                "Data/Textures/Sponza/lion/lion_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/lion/lion_NRM.jpeg",
                "Data/Textures/Sponza/lion/lion_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "roof",
                "Data/Textures/Sponza/roof/roof_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/roof/roof_NRM.jpeg",
                "Data/Textures/Sponza/roof/roof_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "vase",
                "Data/Textures/Sponza/vase/vase_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/vase/vase_NRM.jpeg",
                "Data/Textures/Sponza/vase/vase_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "vase_hanging",
                "Data/Textures/Sponza/vase_hanging/vase_hanging_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/vase_hanging/vase_hanging_NRM.jpeg",
                "Data/Textures/Sponza/vase_hanging/vase_hanging_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "vase_plant",
                "Data/Textures/Sponza/vase_plant/vase_plant_COLOR.jpeg",
                "",
                "",
                "Data/Textures/Sponza/vase_plant/vase_plant_SPEC.jpeg",
                "",
                "",
                "Data/Textures/Sponza/vase_plant/vase_plant_MASK.jpeg",
                "",
                "",
                true,
                1.0f,
                0.5f
            },
            {
                "vase_round",
                "Data/Textures/Sponza/vase_round/vase_round_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/vase_round/vase_round_NRM.jpeg",
                "Data/Textures/Sponza/vase_round/vase_round_SPEC.jpeg",
                "",
                "",
                "",
                "",
                "",
                true,
                1.0f,
                0.5f
            }
        };

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;
        textureImportOptions->MaxMip = 10;

        TextureSurface surface;
        surface.MipLevel = 0;
        surface.NumMipLevels = 10;

        auto materialFunction = [&](SponzaMaterialData& material)
        {
            material.MaterialProp.SpecularPower = 16.0f;
            material.MaterialProp.AlphaThreshold = material.AlphaTreshold;
            material.MaterialProp.Transparency = material.Opacity;
            material.MaterialProp.Reflection = material.ReflectionValue;
            material.MaterialProp.Refraction = material.RefractionValue;
            material.MaterialProp.IndexOfRefraction = material.IndexOfRefractionValue;
            material.MaterialProp.Emissive = material.EmissiveColor;

            if (material.Diffuse != "")
            {
                material.DiffuseTexture = gResourceManager().Load<Texture>(material.Diffuse, textureImportOptions);
                material.MaterialProp.UseDiffuseMap = true;
            }

            if (material.Emissive != "")
            {
                material.EmissiveTexture = gResourceManager().Load<Texture>(material.Emissive, textureImportOptions);
                material.MaterialProp.UseEmissiveMap = true;
            }

            if (material.Normal != "")
            {
                material.NormalTexture = gResourceManager().Load<Texture>(material.Normal, textureImportOptions);
                material.MaterialProp.UseNormalMap = true;
            }

            if (material.Specular != "")
            {
                material.SpecularTexture = gResourceManager().Load<Texture>(material.Specular, textureImportOptions);
                material.MaterialProp.UseSpecularMap = true;
            }

            if (material.Bump != "")
            {
                material.BumpTexture = gResourceManager().Load<Texture>(material.Bump, textureImportOptions);
                material.MaterialProp.UseBumpMap = true;
            }

            if (material.Parallax != "")
            {
                material.ParallaxTexture = gResourceManager().Load<Texture>(material.Parallax, textureImportOptions);
                material.MaterialProp.UseParallaxMap = true;
            }

            if (material.Transparency != "")
            {
                material.TransparencyTexture = gResourceManager().Load<Texture>(material.Transparency, textureImportOptions);
                material.MaterialProp.UseTransparencyMap = true;
            }

            if (material.Occlusion != "")
            {
                material.OcclusionTexture = gResourceManager().Load<Texture>(material.Occlusion, textureImportOptions);
                material.MaterialProp.UseOcclusionMap = true;
            }

            if (material.Reflection != "")
            {
                material.ReflectionTexture = gResourceManager().Load<Texture>(material.Reflection, textureImportOptions);
                material.MaterialProp.UseReflectionMap = true;
            }

            if (material.Environment)
            {
                material.EnvironmentTexture = _loadedSkyboxTexture;
                material.MaterialProp.UseEnvironmentMap = true;
            }

            if(material.Opacity < 1.0f)
                material.MaterialElement = Material::Create(_shaderTransparent);
            else
                material.MaterialElement = Material::Create(_shaderOpaque);

            //float g = 0.9f;
            //material.MaterialElement->SetParam<float>("gTransparency", g);
            material.MaterialElement->SetName(material.Name);
            material.MaterialElement->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));

            if (material.Diffuse != "") material.MaterialElement->SetTexture("DiffuseMap", material.DiffuseTexture, surface);
            if (material.Emissive != "") material.MaterialElement->SetTexture("EmissiveMap", material.EmissiveTexture, surface);
            if (material.Normal != "") material.MaterialElement->SetTexture("NormalMap", material.NormalTexture, surface);
            if (material.Specular != "") material.MaterialElement->SetTexture("SpecularMap", material.SpecularTexture, surface);
            if (material.Parallax != "") material.MaterialElement->SetTexture("ParallaxMap", material.ParallaxTexture, surface);
            if (material.Bump != "") material.MaterialElement->SetTexture("BumpMap", material.BumpTexture, surface);
            if (material.Transparency != "") material.MaterialElement->SetTexture("TransparencyMap", material.TransparencyTexture, surface);
            if (material.Occlusion != "") material.MaterialElement->SetTexture("OcclusionMap", material.OcclusionTexture, surface);
            if (material.Reflection != "") material.MaterialElement->SetTexture("ReflectionMap", material.ReflectionTexture, surface);
            if (material.Environment) material.MaterialElement->SetTexture("EnvironmentMap", material.EnvironmentTexture);

            material.MaterialElement->SetProperties(material.MaterialProp);
        };

        UINT8 initializedMaterialCounter = 0;
        Vector<Thread> materialThreads;
        materialThreads.reserve(8);

        while (initializedMaterialCounter < _materials.size())
        {
            UINT8 rowInitializedMaterialCounter = 0;
            for (UINT8 j = 0; j < 8; j++)
            {
                if (initializedMaterialCounter == _materials.size())
                    break;

                auto materialLoadFunction = std::bind(materialFunction, std::ref(_materials[initializedMaterialCounter]));
                materialThreads.emplace_back(Thread(materialLoadFunction));
                rowInitializedMaterialCounter++;
                initializedMaterialCounter++;
            }

            for (UINT8 j = 0; j < rowInitializedMaterialCounter; j++)
            {
                materialThreads[j].join();
            }

            materialThreads.clear();
        }

        MaterialProperties properties;

        properties.Emissive = Color(1.0f, 0.9f, 0.7f, 1.0f);
        _lightMaterial = Material::Create(_shaderOpaque);
        _lightMaterial->SetProperties(properties);

        _loadedTextureMonkey = gResourceManager().Load<Texture>("Data/Textures/Monkey/diffuse.png", textureImportOptions);
        properties.Emissive = Color(0.0f, 0.0f, 0.0f, 1.0f);
        properties.UseDiffuseMap = true;
        properties.UseEnvironmentMap = true;
        properties.SpecularPower = 128.0f;
        properties.Specular = Color(1.0f, 1.0f, 1.0f, 1.0);
        properties.Reflection = 0.4f;
        properties.Refraction = 0.1f;
        properties.IndexOfRefraction = 1.5f;
        _monkeyMaterial = Material::Create(_shaderOpaque);
        _monkeyMaterial->SetProperties(properties);
        _monkeyMaterial->SetName("Material");
        _monkeyMaterial->SetTexture("DiffuseMap", _loadedTextureMonkey, surface);
        _monkeyMaterial->SetTexture("EnvironmentMap", _loadedSkyboxTexture);
        _monkeyMaterial->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
        _monkeyMaterial->SetProperties(properties);
#endif
    }

    void Application::InitMesh()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = true;
        meshImportOptions->ImportTangents = true;
        meshImportOptions->CpuCached = false;

        _sponzaMesh = gResourceManager().Load<Mesh>("Data/Meshes/Sponza/sponza.obj", meshImportOptions);
        _lightMesh = gResourceManager().Load<Mesh>("Data/Meshes/Sphere/sphere.obj", meshImportOptions);
        _monkeyMesh = gResourceManager().Load<Mesh>("Data/Meshes/Monkey/monkey-hd.obj", meshImportOptions);
#endif
    }

    void Application::InitScene()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        _sceneCameraSO = SceneObject::Create("SceneCamera");
        _sceneCameraFlyer = _sceneCameraSO->AddComponent<CCameraFlyer>();
        _sceneCamera = _sceneCameraSO->AddComponent<CCamera>();
        _sceneCamera->GetViewport()->SetClearColorValue(Color(0.17f, 0.64f, 1.0f, 1.0f));
        _sceneCamera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        _sceneCamera->SetMSAACount(_window->GetDesc().MultisampleCount);
        _sceneCamera->SetHorzFOV(Radian(1.65f));
        _sceneCamera->SetMain(true);
        _sceneCamera->Initialize();

        _sceneSkyboxSO = SceneObject::Create("Skybox");
        _skybox = _sceneSkyboxSO->AddComponent<CSkybox>();
        _skybox->SetTexture(_loadedSkyboxTexture);
        _skybox->Initialize();

        _sceneSponzaSO = SceneObject::Create("Sponza");
        _sponzaRenderable = _sceneSponzaSO->AddComponent<CRenderable>();
        _sponzaRenderable->SetMesh(_sponzaMesh);

        for (auto& material : _materials)
            _sponzaRenderable->SetMaterial(material.Name, material.MaterialElement);
        _sponzaRenderable->Initialize();

        _sceneMonkeySO = SceneObject::Create("Monkey");
        _monkeyRenderable = _sceneMonkeySO->AddComponent<CRenderable>();
        _monkeyRenderable->SetMesh(_monkeyMesh);
        _monkeyRenderable->SetMaterial(_monkeyMaterial);
        _monkeyRenderable->Initialize();
        //_sceneMonkeySO->SetPosition(Vector3(2.0f, -3.05f, -28.0f));
        _sceneMonkeySO->SetPosition(Vector3(2.0f, -3.05f, -12.0f));

        for (INT32 i = -1; i < 2; i++)
        {
            _scenePointLightSOs.push_back(SceneObject::Create("PointLight"));
            HSceneObject lightSO = _scenePointLightSOs.back();
            HLight light = lightSO->AddComponent<CLight>(LightType::Radial);
            light->SetIntensity(25.0f);
            light->SetAttenuationRadius(1.0f);
            light->SetLinearAttenuation(0.35f);
            light->SetQuadraticAttenuation(0.44f);
            light->SetColor(Color(1.0f, 0.95f, 0.85f));
            lightSO->SetPosition(Vector3(0.0f + i * 2.5f, -0.5f, i * 15.0f));

            HRenderable mesh = lightSO->AddComponent<CRenderable>();
            mesh->SetMesh(_lightMesh);
            mesh->SetMaterial(_lightMaterial);
            mesh->Initialize();
        }

        _sceneCameraSO->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 1.5f, -3.0f));

        auto settings = _sceneCamera->GetRenderSettings();
        settings->ExposureScale = 1.2f;
        settings->Gamma = 0.8f;
        settings->Contrast = 1.65f;
        settings->Brightness = -0.05f;
        settings->Bloom.Intensity = 0.5f;

        _sceneDirectionalLightSO = SceneObject::Create("DirectionalLight");
        _directionalLight = _sceneDirectionalLightSO->AddComponent<CLight>(LightType::Directional);
        _directionalLight->SetIntensity(0.075f);
        _directionalLight->Initialize();
        _sceneDirectionalLightSO->Rotate(Vector3(0.0f, 1.0f, 1.0f), -Radian(Math::HALF_PI));
#endif
    }

    void Application::PostStartUp()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        InitInputHandling();
        InitShader();
        InitMaterials();
        InitMesh();
        InitScene();

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
        _sceneMonkeySO->Rotate(Vector3(0.0f, 1.0f, 0.0f), Radian(2.0f * gTime().GetFrameDelta()));

        for (INT32 i = 0; i <= 2; i++)
        {
            float move = 1.5;
            if (i % 2) move = -1.5;

            _scenePointLightSOs[i]->MoveRelative(Vector3(move, 0.0f, 0.0f));
            _scenePointLightSOs[i]->Rotate(Vector3(0.0f, 1.0f, 0.0f), Radian(2.0f * gTime().GetFrameDelta()));
            _scenePointLightSOs[i]->MoveRelative(Vector3(-move, 0.0f, 0.0f));
        }
#endif
    }

    void Application::PostUpdate()
    { }
}
