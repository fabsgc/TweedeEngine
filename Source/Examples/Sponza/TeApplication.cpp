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

#include "Material/TeMaterial.h"
#include "Material/TeShader.h"

namespace te
{
    struct PerInstanceData
    {
        Matrix4 gMatWorld;
        Matrix4 gMatInvWorld;
        Matrix4 gMatWorldNoScale;
        Matrix4 gMatInvWorldNoScale;
        Matrix4 gMatPrevWorld;
        UINT32  gLayer;
    };

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
                1.0f,
                0.5f
            },
            {
                "bricks",
                "Data/Textures/Sponza/bricks/bricks_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/bricks/bricks_NRM.jpeg",
                "Data/Textures/Sponza/bricks/bricks_SPEC.jpeg",
                "",
                "",
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
                "Data/Textures/Sponza/chain/chain_MASK.jpeg",
                0.9f,
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
                0.5f,
                0.2f
            },
            {
                "fabric_f",
                "Data/Textures/Sponza/fabric_f/fabric_f_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/fabric_c/fabric_c_NRM.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_SPEC.jpeg",
                "",
                "",
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
                1.0f,
                0.5f
            },
            {
                "floor",
                "Data/Textures/Sponza/floor/floor_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/floor/floor_NRM.jpeg",
                "Data/Textures/Sponza/floor/floor_SPEC.jpeg",
                "",
                "",
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
                "Data/Textures/Sponza/leaf/leaf_MASK.jpeg",
                0.9f,
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
                "Data/Textures/Sponza/vase_plant/vase_plant_MASK.jpeg",
                0.9f,
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
            material.MaterialProp.Ambient = Color(1.0f, 1.0f, 1.0f, 0.75f);
            //material.MaterialProp.Emissive = Color(1.0f, 1.0f, 1.0f, 0.75f);
            material.MaterialProp.AlphaThreshold = material.AlphaTreshold;
            material.MaterialProp.Transparency = material.Opacity;

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

            if (material.Transparency != "")
            {
                material.TransparencyTexture = gResourceManager().Load<Texture>(material.Transparency, textureImportOptions);
                material.MaterialProp.UseTransparencyMap = true;
            }

            if(material.Opacity < 1.0f)
                material.MaterialElement = Material::Create(_shaderTransparent);
            else
                material.MaterialElement = Material::Create(_shaderOpaque);

            float g = 0.9f;

            material.MaterialElement->SetParam<float>("gTransparency", g);
            material.MaterialElement->SetName(material.Name);
            material.MaterialElement->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetAnisotropicSamplerState());

            if (material.Diffuse != "") material.MaterialElement->SetTexture("DiffuseMap", material.DiffuseTexture, surface);
            if (material.Emissive != "") material.MaterialElement->SetTexture("EmissiveMap", material.EmissiveTexture, surface);
            if (material.Normal != "") material.MaterialElement->SetTexture("NormalMap", material.NormalTexture, surface);
            if (material.Specular != "") material.MaterialElement->SetTexture("SpecularMap", material.SpecularTexture, surface);
            if (material.Bump != "") material.MaterialElement->SetTexture("BumpMap", material.BumpTexture, surface);
            if (material.Transparency != "") material.MaterialElement->SetTexture("TransparencyMap", material.TransparencyTexture, surface);

            material.MaterialElement->SetProperties(material.MaterialProp);
        };

        //materialFunction(_materials[16]);

        for (auto& material : _materials)
        {
            materialFunction(material);
        }
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

        TE_PRINT((_sponzaMesh.GetHandleData())->data);
        TE_PRINT((_sponzaMesh.GetHandleData())->uuid.ToString());
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
        _sceneCamera->SetMSAACount(8);
        _sceneCamera->SetMain(true);
        _sceneCamera->Initialize();

        _sceneSponzaSO = SceneObject::Create("Sponza");
        _sponzaRenderable = _sceneSponzaSO->AddComponent<CRenderable>();
        _sponzaRenderable->SetMesh(_sponzaMesh);
        
        for (auto& material : _materials)
        {
            _sponzaRenderable->SetMaterial(material.Name, material.MaterialElement);
        }

        //_sponzaRenderable->SetMaterial(_materials[16].MaterialElement);

        _sponzaRenderable->Initialize();

        _sceneCameraSO->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 1.5f, -3.0f));
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
    { }

    void Application::PostUpdate()
    { }
}
