#pragma once

#include "TeCorePrerequisites.h"
#include "TeCoreApplication.h"
#include "Material/TeMaterial.h"

namespace te
{
    /**
     * Represents the primary entry point for the core systems. Handles start-up, shutdown, primary loop and allows you to
     * load and unload plugins.
     */
    class Application : public CoreApplication
    {
    public:
        Application(START_UP_DESC desc) : CoreApplication(desc) {}
        virtual ~Application() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(Application)

        /** Starts the framework. If using a custom Application system, provide it as a template parameter. */
        template<class T = Application>
        static void StartUp(const START_UP_DESC& desc)
        {
            CoreApplication::StartUp<T>(desc);
        }

    protected:
        /** @copydoc CoreApplication::PostStartUp */
        void PostStartUp() override;

        /** @copydoc CoreApplication::PreShutDown */
        void PreShutDown() override;

        /** @copydoc CoreApplication::PreUpdate */
        void PreUpdate() override;

        /** @copydoc CoreApplication::PreUpdate */
        void PostUpdate() override;

    protected:
        struct SponzaMaterialData
        {
            String Name;
            String Diffuse;
            String Emissive;
            String Normal;
            String Specular;
            String Bump;
            String Parallax;
            String Transparency;
            String Occlusion;
            String Reflection;
            float  Opacity = 1.0f;
            float  AlphaTreshold = 0.1f;
            Color EmissiveColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

            HTexture DiffuseTexture;
            HTexture EmissiveTexture;
            HTexture NormalTexture;
            HTexture SpecularTexture;
            HTexture BumpTexture;
            HTexture ParallaxTexture;
            HTexture TransparencyTexture;
            HTexture OcclusionTexture;
            HTexture ReflectionTexture;

            HMaterial MaterialElement;

            MaterialProperties MaterialProp;
        };

        void InitInputHandling();
        void InitShader();
        void InitMaterials();
        void InitMesh();
        void InitScene();

    protected:
#if TE_PLATFORM == TE_PLATFORM_WIN32
        Vector<SponzaMaterialData> _materials;

        HTexture _loadedSkyboxTexture;

        HShader _shaderOpaque;
        HShader _shaderTransparent;

        HCamera _sceneCamera;
        HCameraFlyer _sceneCameraFlyer;
        HMesh _sponzaMesh;
        HRenderable _sponzaRenderable;
        HSkybox _skybox;
        HLight _pointLight;

        HSceneObject _sceneCameraSO;
        HSceneObject _sceneSponzaSO;
        HSceneObject _sceneSkyboxSO;
        HSceneObject _scenePointLightSO;
#endif
    };
}
