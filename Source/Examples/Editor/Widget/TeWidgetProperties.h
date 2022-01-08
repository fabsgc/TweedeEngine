#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"
#include "../TeEditor.h"

namespace te
{
    class WidgetProperties : public Widget
    {
    public:
        WidgetProperties();
        ~WidgetProperties();

        /** @copydoc Widget::Initialize */
        virtual void Initialize() override;

        /** @copydoc Widget::Update */
        virtual void Update() override;

        /** @copydoc Widget::UpdateBackground */
        virtual void UpdateBackground() override;

    protected:
        /** Shows element properties, returns true if something has been modified */
        bool ShowSceneObjectProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCameraProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCameraFlyerProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCameraUIProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCLightProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCRenderableProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCScriptProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCSkyboxProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCAnimationProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCBoneProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCAudioListenerProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCAudioSourceProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCRigidBodyProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCSoftBodyProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCMeshSoftBodyProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCEllipsoidSoftBodyProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCRopeSoftBodyProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCPatchSoftBodyProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCBoxColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCapsuleColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCConeColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCylinderColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCMeshColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCPlaneColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCSphereColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCHeightFieldColliderProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCConeTwistJointProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCD6JointProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCHingeJointProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCSliderJointProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCSphericalJointProperties();

        /** @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCDecal();

        /**  Show transform and mobility options */
        bool ShowTransform(Transform& transform, ObjectMobility& mobility, bool disableTransform = false);

        /**  Show information about a GameObject (both Component and SceneObject) */
        bool ShowGameObjectInformation(const SPtr<GameObject>& gameObject);

        /** Show information about a camera */
        bool ShowCamera(SPtr<CCamera> camera);

        /** Show information about a cameraFlyer */
        bool ShowCameraFlyer(SPtr<CCameraFlyer> cameraFlyer);

        /** Show information about a cameraUI */
        bool ShowCamerUI(SPtr<CCameraUI> cameraUI);

        /** Show information about a light */
        bool ShowLight(SPtr<Light> light);

        /** Show information about light shadow casting */
        bool ShowLightShadow(SPtr<Light> light);

        /** Show information about renderable properties */
        bool ShowRenderableProperties(SPtr<Renderable> renderable);

        /** Show information about renderable properties */
        bool ShowRenderableSubMeshes(SPtr<Renderable> renderable);

        /** Show information about a script */
        bool ShowScript(SPtr<CScript> script);

        /** Show information about an animation */
        bool ShowAnimation(SPtr<CAnimation> animation);

        /** Show information about animation clips */
        bool ShowAnimationClips(SPtr<CAnimation> animation);

        /** Show information about a skybox */
        bool ShowSkybox(SPtr<Skybox> skybox);

        /** Show information about a body */
        bool ShowBody(SPtr<CBody> body);

        /** Show information about a body */
        bool ShowCollider(SPtr<CCollider> collider);

        /** Show information about a joint */
        bool ShowJoint(SPtr<CJoint> joint);

        /** Show FileBrowser in order to load a new mesh */
        bool ShowLoadMesh();

        /** Show FileBrowser in order to load a new cubemap texture */
        bool ShowLoadSkybox();

        /** Show FileBrowser in order to load a new script */
        bool ShowLoadScript();

        /** Show FileBrowser in order to load a new audio clip */
        bool ShowLoadAudioClip();

        /** Show FileBrowser in order to load a new texture for a PhysicsHeightField */
        bool ShowLoadHeightFieldTexture();

        /** Try to find an already existing PhysicsHeightField using given texture, or create a new one */
        HPhysicsHeightField GetOrCreatePhysicsHightFieldFromTex(SPtr<Texture> texture);

    protected:
        Editor::SelectionData& _selections;
        ImGuiFileBrowser& _fileBrowser;

        bool _loadMesh = false;
        bool _loadPhysicsMesh = false;
        bool _loadSkybox = false;
        bool _loadSkyboxIrradiance = false;
        bool _loadScript = false;
        bool _loadAudioClip = false;
        bool _loadHeightFieldTexture = false;
    };
}
