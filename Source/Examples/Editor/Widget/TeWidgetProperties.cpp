#include "TeWidgetProperties.h"

#include "../TeEditorResManager.h"
#include "../TeEditorUtils.h"
#include "Resources/TeResourceManager.h"
#include "Resources/TeBuiltinResources.h"
#include "String/TeUnicode.h"
#include "Utility/TeFileSystem.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeTransform.h"
#include "Mesh/TeMesh.h"
#include "RenderAPI/TeSubMesh.h"
#include "Physics/TePhysicsMesh.h"
#include "Physics/TePhysicsHeightField.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Scripting/TeScriptManager.h"
#include "Audio/TeAudioClip.h"
#include "Audio/TeAudioClipImportOptions.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCCamera.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCScript.h"
#include "Components/TeCLight.h"
#include "Components/TeCAnimation.h"
#include "Components/TeCBone.h"
#include "Components/TeCAudioListener.h"
#include "Components/TeCAudioSource.h"
#include "Components/TeCRigidBody.h"
#include "Components/TeCSoftBody.h"
#include "Components/TeCBoxCollider.h"
#include "Components/TeCCapsuleCollider.h"
#include "Components/TeCConeCollider.h"
#include "Components/TeCCylinderCollider.h"
#include "Components/TeCMeshCollider.h"
#include "Components/TeCPlaneCollider.h"
#include "Components/TeCSphereCollider.h"
#include "Components/TeCHeightFieldCollider.h"
#include "Physics/TePhysics.h"

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wsign-compare" 
#endif

namespace te
{
    WidgetProperties::WidgetProperties()
        : Widget(WidgetType::Properties)
        , _selections(gEditor().GetSelectionData())
        , _fileBrowser(gEditor().GetFileBrowser())
    { 
        _title = PROPERTIES_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetProperties::~WidgetProperties()
    { }

    void WidgetProperties::Initialize()
    { }

    void WidgetProperties::Update()
    {
        bool hasChanged = false;

        if (_selections.ClickedComponent)
        {
            SPtr<GameObject> gameObject = std::static_pointer_cast<GameObject>(_selections.ClickedComponent);
            if (ShowGameObjectInformation(gameObject))
                hasChanged = true;

            switch (_selections.ClickedComponent->GetCoreType())
            {
            case TID_CCamera:
            {
                if (ShowCCameraProperties())
                    hasChanged = true;
            }
            break;

            case TID_CRenderable:
            {
                if (ShowCRenderableProperties())
                    hasChanged = true;
            }
            break;

            case TID_CLight:
            {
                if (ShowCLightProperties())
                    hasChanged = true;
            }
            break;

            case TID_CSkybox:
            {
                if (ShowCSkyboxProperties())
                    hasChanged = true;
            }
            break;

            case TID_CCameraFlyer:
            {
                if (ShowCCameraFlyerProperties())
                    hasChanged = true;
            }
            break;

            case TID_CCameraUI:
            {
                if (ShowCCameraUIProperties())
                    hasChanged = true;
            }
            break;

            case TID_CScript:
            {
                if (ShowCScriptProperties())
                    hasChanged = true;
            }
            break;

            case TID_CAnimation:
            {
                if (ShowCAnimationProperties())
                    hasChanged = true;
            }
            break;

            case TID_CBone:
            {
                if (ShowCBoneProperties())
                    hasChanged = true;
            }
            break;

            case TID_CAudioListener:
            {
                if (ShowCAudioListenerProperties())
                    hasChanged = true;
            }
            break;

            case TID_CAudioSource:
            {
                if (ShowCAudioSourceProperties())
                    hasChanged = true;
            }
            break;

            case TID_CRigidBody:
            {
                if (ShowCRigidBodyProperties())
                    hasChanged = true;
            }
            break;

            case TID_CSoftBody:
            {
                if (ShowCSoftBodyProperties())
                    hasChanged = true;
            }
            break;

            case TID_CBoxCollider:
            {
                if (ShowCBoxColliderProperties())
                    hasChanged = true;
            }
            break;

            case TID_CCapsuleCollider:
            {
                if (ShowCCapsuleColliderProperties())
                    hasChanged = true;
            }
            break;

            case TID_CConeCollider:
            {
                if (ShowCConeColliderProperties())
                    hasChanged = true;
            }
            break;

            case TID_CHeightFieldCollider:
            {
                if (ShowCHeightFieldColliderProperties())
                    hasChanged = true;
            }
            break;

            case TID_CCylinderCollider:
            {
                if (ShowCCylinderColliderProperties())
                    hasChanged = true;
            }
            break;

            case TID_CMeshCollider:
            {
                if (ShowCMeshColliderProperties())
                    hasChanged = true;
            }
            break;

            case TID_CPlaneCollider:
            {
                if (ShowCPlaneColliderProperties())
                    hasChanged = true;
            }
            break;

            case TID_CSphereCollider:
            {
                if (ShowCSphereColliderProperties())
                    hasChanged = true;
            }
            break;

            default:
                break;
            }
        }
        else if (_selections.ClickedSceneObject)
        {
            if (ShowSceneObjectProperties())
                hasChanged = true;
        }

        if (hasChanged)
        {
            gEditor().NeedsRedraw();
            gEditor().GetSettings().State = Editor::EditorState::Modified;
        }
    }

    void WidgetProperties::UpdateBackground()
    { }

    bool WidgetProperties::ShowSceneObjectProperties()
    {
        bool hasChanged = false;
        ObjectMobility mobility = _selections.ClickedSceneObject->GetMobility();
        Transform transform = _selections.ClickedSceneObject->GetLocalTransform();
        SPtr<GameObject> gameObject = std::static_pointer_cast<GameObject>(_selections.ClickedSceneObject);

        if (ShowGameObjectInformation(gameObject))
            hasChanged = true;

        bool active = _selections.ClickedSceneObject->GetActive(true);
        if (ImGuiExt::RenderOptionBool(active, "##scene_object_information_active_option", "Active"))
        {
            hasChanged = true;
            _selections.ClickedSceneObject->SetActive(active);
        }

        if (ShowTransform(transform, mobility))
        {
            _selections.ClickedSceneObject->SetLocalTransform(transform);
            _selections.ClickedSceneObject->SetMobility(mobility);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraProperties()
    {
        bool hasChanged = false;
        SPtr<CCamera> camera = std::static_pointer_cast<CCamera>(_selections.ClickedComponent);
        ObjectMobility mobility = camera->_getCamera()->GetMobility();
        Transform transform = camera->GetSceneObject()->GetTransform();

        if (ShowTransform(transform, mobility))
        {
            camera->_getCamera()->SetMobility(mobility);
            camera->GetSceneObject()->SetLocalTransform(transform);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Rendering Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowCamera(camera))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraFlyerProperties()
    {
        bool hasChanged = false;
        SPtr<CCameraFlyer> camera = std::static_pointer_cast<CCameraFlyer>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Flying Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowCameraFlyer(camera))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCameraUIProperties()
    {
        bool hasChanged = false;
        SPtr<CCameraUI> camera = std::static_pointer_cast<CCameraUI>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Orbital Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowCamerUI(camera))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCLightProperties()
    {
        bool hasChanged = false;
        SPtr<CLight> light = std::static_pointer_cast<CLight>(_selections.ClickedComponent);
        ObjectMobility mobility = light->_getLight()->GetMobility();
        Transform transform = light->GetSceneObject()->GetTransform();

        if (ShowTransform(transform, mobility))
        {
            light->_getLight()->SetMobility(mobility);
            light->GetSceneObject()->SetLocalTransform(transform);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowLight(light->_getLight()))
                hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Shadow", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowLightShadow(light->_getLight()))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCRenderableProperties()
    {
        bool hasChanged = false;
        SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(_selections.ClickedComponent);
        ObjectMobility mobility = renderable->_getInternal()->GetMobility();
        Transform transform = renderable->GetSceneObject()->GetTransform();

        if (ShowTransform(transform, mobility))
        {
            renderable->_getInternal()->SetMobility(mobility);
            renderable->GetSceneObject()->SetLocalTransform(transform);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowRenderableProperties(renderable->_getInternal()))
                hasChanged = true;
        }

        if (renderable->GetMesh())
        {
            if (ImGui::CollapsingHeader("SubMeshes", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ShowRenderableSubMeshes(renderable->_getInternal()))
                    hasChanged = true;
            }
        }
        
        return hasChanged;
    }

    bool WidgetProperties::ShowCScriptProperties()
    {
        bool hasChanged = false;
        SPtr<CScript> script = std::static_pointer_cast<CScript>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowScript(script))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCAnimationProperties()
    {
        bool hasChanged = false;
        SPtr<CAnimation> animation = std::static_pointer_cast<CAnimation>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowAnimation(animation))
                hasChanged = true;
        }

        if (animation->GetNumClips() > 0)
        {
            if (ShowAnimationClips(animation))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCBoneProperties()
    {
        bool hasChanged = false;
        SPtr<CBone> bone = std::static_pointer_cast<CBone>(_selections.ClickedComponent);
        ObjectMobility mobility = bone->GetSceneObject()->GetMobility();
        Transform transform = bone->GetSceneObject()->GetTransform();
        const float width = ImGui::GetWindowContentRegionWidth() - 120.0f;

        if (ShowTransform(transform, mobility))
        {
            bone->GetSceneObject()->SetMobility(mobility);
            bone->GetSceneObject()->SetLocalTransform(transform);
            hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Bone", ImGuiTreeNodeFlags_DefaultOpen))
        {
            HSceneObject currentSO = bone->GetSceneObject();
            HRenderable renderable;
            HAnimation animation;

            while (!currentSO.Empty())
            {
                renderable = static_object_cast<CRenderable>(currentSO->GetComponent<CRenderable>());
                animation = static_object_cast<CAnimation>(currentSO->GetComponent<CAnimation>());

                if (!renderable.Empty() && !animation.Empty()) // If there is an animation and a renderable in the hierarchy
                    break;

                if (currentSO == gEditor().GetSceneRoot())
                    break;

                currentSO = currentSO->GetParent();           
            }

            if (!renderable.Empty() && !animation.Empty())
            {
                if (renderable->GetMesh() && renderable->GetMesh()->GetSkeleton()) // if the renderable has a skeleton
                {
                    ImGuiExt::ComboOptions<UINT32> bonesOptions;
                    SPtr<Skeleton> skeleton = renderable->GetMesh()->GetSkeleton();
                    UINT32 boneIndex = static_cast<UINT32>(-1);

                    bonesOptions.AddOption(static_cast<UINT32>(-1), "None");

                    for (UINT32 i = 0; i < skeleton->GetNumBones(); i++)
                    {
                        SkeletonBoneInfo boneInfo = skeleton->GetBoneInfo(i);
                        bonesOptions.AddOption(i, boneInfo.Name);

                        if (boneInfo.Name == bone->GetBoneName())
                            boneIndex = i;
                    }

                    if (ImGuiExt::RenderOptionCombo<UINT32>(&boneIndex, "##bone_name_option", "Mimicked bone", bonesOptions, width))
                    {
                        if (boneIndex != -1)
                            bone->SetBoneName(skeleton->GetBoneInfo(boneIndex).Name);
                        else
                            bone->SetBoneName("");

                        hasChanged = true;
                    }
                }
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCAudioListenerProperties()
    {
        bool hasChanged = false;
        SPtr<CAudioListener> audioListener = std::static_pointer_cast<CAudioListener>(_selections.ClickedComponent);

        return hasChanged;
    }

    bool WidgetProperties::ShowCAudioSourceProperties()
    {
        bool hasChanged = false;
        SPtr<CAudioSource> audioSource = std::static_pointer_cast<CAudioSource>(_selections.ClickedComponent);
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;
        HAudioClip clip = audioSource->GetClip();

        if (ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGuiExt::ComboOptions<UUID> audioClipsOptions;
            UUID emptyAudioClip = UUID(50, 0, 0, 0);
            UUID loadAudioClip = UUID::EMPTY;
            UUID audioClipUUID = (clip.IsLoaded()) ? clip->GetUUID() : emptyAudioClip;
            EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<AudioClip>();

            // Play/Stop
            {
                ImGui::PushID("##audio_source_play_stop");

                if (audioSource->GetState() == AudioSourceState::Playing)
                {
                    if (ImGui::Button(ICON_FA_STOP " Stop", ImVec2(ImGui::GetWindowContentRegionWidth(), 25.0f)))
                        audioSource->Stop();
                }
                else
                {
                    if (ImGui::Button(ICON_FA_PLAY " Play", ImVec2(ImGui::GetWindowContentRegionWidth(), 25.0f)))
                        audioSource->Play();
                }
                ImGui::PopID();
            }
            ImGui::Separator();

            // Play3D
            {
                bool isPlay3D = audioSource->GetIsPlay3D();
                if (ImGuiExt::RenderOptionBool(isPlay3D, "##audio_source_is_play_3d", "Play 3D"))
                {
                    hasChanged = true;
                    audioSource->SetIsPlay3D(isPlay3D);
                }
            }

            // IsLooping
            {
                bool isLooping = audioSource->GetIsLooping();
                if (ImGuiExt::RenderOptionBool(isLooping, "##audio_source_is_looping", "Is looping"))
                {
                    hasChanged = true;
                    audioSource->SetIsLooping(isLooping);
                }
            }

            // PlayOnStart
            {
                bool playOnStart = audioSource->GetPlayOnStart();
                if (ImGuiExt::RenderOptionBool(playOnStart, "##audio_source_play_on_start", "Play on start"))
                {
                    hasChanged = true;
                    audioSource->SetPlayOnStart(playOnStart);
                }
            }
            ImGui::Separator();

            // current audio clip to use
            {
                for (auto& resource : container.Res)
                    audioClipsOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());

                audioClipsOptions.AddOption(emptyAudioClip, ICON_FA_TIMES_CIRCLE " No clip");
                audioClipsOptions.AddOption(loadAudioClip, ICON_FA_FOLDER_OPEN " Load");

                if (ImGuiExt::RenderOptionCombo<UUID>(&audioClipUUID, "##audio_clip_option", "Audio Clip", audioClipsOptions, width))
                {
                    if (audioClipUUID == loadAudioClip)
                    {
                        _loadAudioClip = true;
                    }
                    else if (audioClipUUID == emptyAudioClip)
                    {
                        audioSource->SetClip(HAudioClip());
                        hasChanged = true;
                    }
                    else if (audioClipUUID != ((clip.IsLoaded()) ? clip->GetUUID() : emptyAudioClip))
                    {
                        audioSource->SetClip(gResourceManager().Load<AudioClip>(audioClipUUID));
                        hasChanged = true;
                    }
                }
            }
            ImGui::Separator();

            // Volume
            {
                float volume = audioSource->GetVolume();
                if (ImGuiExt::RenderOptionFloat(volume, "##audio_source_volume", "Volume", 0.0f, 1.0f, width))
                {
                    hasChanged = true;
                    audioSource->SetVolume(volume);
                }
            }

            // Pitch
            {
                float pitch = audioSource->GetPitch();
                if (ImGuiExt::RenderOptionFloat(pitch, "##audio_source_pitch", "Pitch", 0.0f, 8.0f, width))
                {
                    hasChanged = true;
                    audioSource->SetPitch(pitch);
                }
            }

            // MinDistance
            {
                float minDistance = audioSource->GetMinDistance();
                if (ImGuiExt::RenderOptionFloat(minDistance, "##audio_source_min_distance", "Min distance", 0.0f, 100.0f, width))
                {
                    hasChanged = true;
                    audioSource->SetMinDistance(minDistance);
                }
            }

            // Attenuation
            {
                float attenuation = audioSource->GetAttenuation();
                if (ImGuiExt::RenderOptionFloat(attenuation, "##audio_source_attenuation", "Attenuation", 0.0f, 100.0f, width))
                {
                    hasChanged = true;
                    audioSource->SetAttenuation(attenuation);
                }
            }
        }

        if (ShowLoadAudioClip())
            hasChanged = true;

        return hasChanged;
    }

    bool WidgetProperties::ShowCSkyboxProperties()
    {
        bool hasChanged = false;
        SPtr<CSkybox> skybox = std::static_pointer_cast<CSkybox>(_selections.ClickedComponent);
        ObjectMobility mobility = skybox->_getSkybox()->GetMobility();
        Transform transform = skybox->_getSkybox()->GetTransform();

        if (ImGui::CollapsingHeader("Skybox", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowSkybox(skybox->_getSkybox()))
                hasChanged = true;
        }

        if (ShowTransform(transform, mobility, true))
        {
            skybox->_getSkybox()->SetMobility(mobility);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCRigidBodyProperties()
    {
        bool hasChanged = false;
        SPtr<CRigidBody> rigidBody = std::static_pointer_cast<CRigidBody>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowBody(rigidBody))
                hasChanged = true;

            ImGui::Separator();

            // Angular Factor
            {
                Vector3 angularFactor = rigidBody->GetAngularFactor();
                if (ImGuiExt::RenderVector3(angularFactor, "##rigid_body_option_angular_factor", " Angular Factor", 60.0f))
                {
                    rigidBody->SetAngularFactor(angularFactor);
                    hasChanged = true;
                }
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCSoftBodyProperties()
    {
        bool hasChanged = false;
        SPtr<CSoftBody> softBody = std::static_pointer_cast<CSoftBody>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Soft Body", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ShowBody(softBody))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCBoxColliderProperties()
    {
        bool hasChanged = false;
        SPtr<CBoxCollider> collider = std::static_pointer_cast<CBoxCollider>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Box collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Extents
            {
                Vector3 extents = collider->GetExtents();
                if (ImGuiExt::RenderVector3(extents, "##collider_option_extents", " Extents", 20.0f))
                {
                    collider->SetExtents(extents);
                    hasChanged = true;
                }
            }

            ImGui::Separator();

            if (ShowCollider(collider))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCapsuleColliderProperties()
    {
        bool hasChanged = false;
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;
        SPtr<CCapsuleCollider> collider = std::static_pointer_cast<CCapsuleCollider>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Capsule collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Radius
            {
                float radius = collider->GetRadius();
                if (ImGuiExt::RenderOptionFloat(radius, "##collider_option_radius", " Radius", 0.0f, 512.0f, width))
                {
                    collider->SetRadius(radius);
                    hasChanged = true;
                }
            }

            // Height
            {
                float height = collider->GetHeight();
                if (ImGuiExt::RenderOptionFloat(height, "##collider_option_height", " Height", 0.0f, 512.0f, width))
                {
                    collider->SetHeight(height);
                    hasChanged = true;
                }
            }
            ImGui::Separator();

            if (ShowCollider(collider))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCConeColliderProperties()
    {
        bool hasChanged = false;
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;
        SPtr<CConeCollider> collider = std::static_pointer_cast<CConeCollider>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Cone collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Radius
            {
                float radius = collider->GetRadius();
                if (ImGuiExt::RenderOptionFloat(radius, "##collider_option_radius", " Radius", 0.0f, 512.0f, width))
                {
                    collider->SetRadius(radius);
                    hasChanged = true;
                }
            }

            // Height
            {
                float height = collider->GetHeight();
                if (ImGuiExt::RenderOptionFloat(height, "##collider_option_height", " Height", 0.0f, 512.0f, width))
                {
                    collider->SetHeight(height);
                    hasChanged = true;
                }
            }
            ImGui::Separator();

            if (ShowCollider(collider))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCCylinderColliderProperties()
    {
        bool hasChanged = false;
        SPtr<CCylinderCollider> collider = std::static_pointer_cast<CCylinderCollider>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Cylinder collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Extents
            {
                Vector3 extents = collider->GetExtents();
                if (ImGuiExt::RenderVector3(extents, "##collider_option_extents", " Extents", 20.0f))
                {
                    collider->SetExtents(extents);
                    hasChanged = true;
                }
            }

            ImGui::Separator();

            if (ShowCollider(collider))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCMeshColliderProperties()
    {
        bool hasChanged = false;
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;
        SPtr<CMeshCollider> collider = std::static_pointer_cast<CMeshCollider>(_selections.ClickedComponent);
        HPhysicsMesh mesh = collider->GetMesh();

        if (ImGui::CollapsingHeader("Mesh collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // PhysicsMesh
            {
                ImGuiExt::ComboOptions<UUID> meshesOptions;
                UUID emptyMesh = UUID(50, 0, 0, 0);
                UUID loadPhysicsMesh = UUID::EMPTY;
                UUID meshUUID = (mesh.IsLoaded()) ? mesh->GetUUID() : emptyMesh;
                EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<PhysicsMesh>();

                // current PhysicsMesh to use
                for (auto& resource : container.Res)
                    meshesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());

                meshesOptions.AddOption(emptyMesh, ICON_FA_TIMES_CIRCLE " No Physic Mesh");
                meshesOptions.AddOption(UUID::EMPTY, ICON_FA_FOLDER_OPEN " Load");

                if (ImGuiExt::RenderOptionCombo<UUID>(&meshUUID, "##collider_physic_mesh_option", "Physic Mesh", meshesOptions, width))
                {
                    if (meshUUID == loadPhysicsMesh)
                    {
                        _loadPhysicsMesh = true;
                    }
                    else if (meshUUID == emptyMesh)
                    {
                        collider->SetMesh(HPhysicsMesh());
                        hasChanged = true;
                    }
                    else if (meshUUID != ((mesh.IsLoaded()) ? mesh->GetUUID() : emptyMesh))
                    {
                        collider->SetMesh(gResourceManager().Load<PhysicsMesh>(meshUUID));
                        hasChanged = true;
                    }
                }
            }
            ImGui::Separator();

            // CollisionType
            {
                static ImGuiExt::ComboOptions<PhysicsMeshType> collisionTypeOptions;
                if (collisionTypeOptions.Options.size() == 0)
                {
                    collisionTypeOptions.AddOption(PhysicsMeshType::Triangle, "Triangle");
                    collisionTypeOptions.AddOption(PhysicsMeshType::Convex, "Convex");
                }

                PhysicsMeshType type = collider->GetCollisionType();

                if (ImGuiExt::RenderOptionCombo<PhysicsMeshType>(&type, "##collider_physic_mesh_type_option", "Collision Type", collisionTypeOptions, width))
                {
                    collider->SetCollisionType(type);
                    hasChanged = true;
                }
            }
            ImGui::Separator();

            if (ShowLoadMesh())
                hasChanged = true;

            if (ShowCollider(collider))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCPlaneColliderProperties()
    {
        bool hasChanged = false;
        SPtr<CPlaneCollider> collider = std::static_pointer_cast<CPlaneCollider>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Plane collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Normal
            {
                Vector3 normal = collider->GetNormal();
                if (ImGuiExt::RenderVector3(normal, "##collider_option_normal", " Normal", 20.0f))
                {
                    collider->SetNormal(normal);
                    hasChanged = true;
                }
            }

            ImGui::Separator();

            if (ShowCollider(collider))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCSphereColliderProperties()
    {
        bool hasChanged = false;
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;
        SPtr<CSphereCollider> collider = std::static_pointer_cast<CSphereCollider>(_selections.ClickedComponent);

        if (ImGui::CollapsingHeader("Sphere collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Radius
            {
                float radius = collider->GetRadius();
                if (ImGuiExt::RenderOptionFloat(radius, "##collider_option_radius", " Radius", 0.0f, 512.0f, width))
                {
                    collider->SetRadius(radius);
                    hasChanged = true;
                }
            }

            if (ShowCollider(collider))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCHeightFieldColliderProperties()
    {
        bool hasChanged = false;
        float width = ImGui::GetWindowContentRegionWidth() - 100.0f;
        SPtr<CHeightFieldCollider> collider = std::static_pointer_cast<CHeightFieldCollider>(_selections.ClickedComponent);
        HPhysicsHeightField heightField = collider->GetHeightField();
        SPtr<Texture> texture = heightField.IsLoaded() ? heightField->GetTexture() : nullptr;
        UINT8 flags = (UINT8)ImGuiExt::ComboOptionFlag::ShowTexture;

        if (ImGui::CollapsingHeader("Terrain collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // HeightScale
            {
                float heightScale = collider->GetHeightScale();
                if (ImGuiExt::RenderOptionFloat(heightScale, "##collider_physic_height_scale_option", "Height Scale", 0.0f, 2.0f, width))
                {
                    hasChanged = true;
                    collider->SetHeightScale(heightScale);
                }
            }
            ImGui::Separator();

            // PhysicsHeightField
            {
                ImGuiExt::ComboOptions<UUID> texturesOptions;
                UUID emptyTexture = UUID(50, 0, 0, 0);
                UUID loadTexture = UUID::EMPTY;
                UUID textureUUID = (texture) ? texture->GetUUID() : emptyTexture;
                EditorResManager::ResourcesContainer& texContainer = EditorResManager::Instance().Get<Texture>();

                // current texture to use as PhysicsHeightField
                for (auto& resource : texContainer.Res)
                {
                    HTexture tex = static_resource_cast<Texture>(resource.second);

                    if (tex.IsLoaded() && tex->GetProperties().GetTextureType() != TextureType::TEX_TYPE_CUBE_MAP)
                    {
                        texturesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName(),
                            std::static_pointer_cast<Texture>(resource.second.GetInternalPtr()));
                    }
                }

                texturesOptions.AddOption(emptyTexture, ICON_FA_TIMES_CIRCLE " No HeightField");
                texturesOptions.AddOption(UUID::EMPTY, ICON_FA_FOLDER_OPEN " Load");

                if (texture && texture->GetProperties().GetTextureType() != TextureType::TEX_TYPE_CUBE_MAP)
                {
                    ImGuiExt::RenderImage(texture, 3, Vector2(26.0f, 26.0f));
                    ImGui::SameLine();

                    ImVec2 cursor = ImGui::GetCursorPos();
                    cursor.x -= 5.0f;
                    ImGui::SetCursorPos(cursor);

                    width -= 26.0f;
                }

                if (ImGuiExt::RenderOptionCombo<UUID>(&textureUUID, "##collider_physic_height_field_option", "Height Field", texturesOptions, width, flags))
                {
                    if (textureUUID == loadTexture)
                    {
                        // Load and create (if not exists) PhysicsHeightField associated to this texture
                        _loadHeightFieldTexture = true;
                    }
                    else if (textureUUID == emptyTexture)
                    {
                        collider->SetHeightField(HPhysicsHeightField());
                        hasChanged = true;
                    }
                    else if (textureUUID != ((heightField.IsLoaded()) ? texture->GetUUID() : emptyTexture))
                    {
                        // Find or create PhysicsHeightField associated to this texture
                        collider->SetHeightField(GetOrCreatePhysicsHightFieldFromTex(gResourceManager().Load<Texture>(textureUUID).GetInternalPtr()));
                        hasChanged = true;
                    }
                }
            }
            ImGui::Separator();

            if (ShowCollider(collider))
                hasChanged = true;

            if (ShowLoadHeightFieldTexture())
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowTransform(Transform& transform, ObjectMobility& mobility, bool disableTransform)
    {
        bool hasChanged = false;
        const float widgetWidth = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static ImGuiExt::ComboOptions<int> mobilityOptions;

            if (mobilityOptions.Options.size() == 0)
            {
                mobilityOptions.AddOption((int)ObjectMobility::Movable, "Movable");
                mobilityOptions.AddOption((int)ObjectMobility::Immovable, "Immovable");
                mobilityOptions.AddOption((int)ObjectMobility::Static, "Static");
            }

            if (ImGuiExt::RenderOptionCombo<int>((int*)(&mobility), "##transform_mobility_option", "Mobility", mobilityOptions, widgetWidth, 
                disableTransform ? (UINT8)ImGuiExt::ComboOptionFlag::Disable : 0))
                hasChanged = true;

            ImGui::Separator();

            if (ImGuiExt::RenderTransform(transform, "transform_option", disableTransform))
                hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowGameObjectInformation(SPtr<GameObject>& gameObject)
    {
        char inputName[256];
        char inputUUID[64];
        bool hasChanged = false;
        String name = gameObject->GetName();
        String uuid = gameObject->GetUUID().ToString();
        const float widgetWidth = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if(name.length() < 256)
            strcpy(inputName, name.c_str());
        else
            strcpy(inputName, name.substr(0,255).c_str());

        strcpy(inputUUID, uuid.c_str());

        if (ImGui::CollapsingHeader("Identification", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(widgetWidth);
            if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
            {
                gameObject->SetName(inputName);
                hasChanged = true;
            }
            ImGui::Separator();

            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
            ImGui::PopItemFlag();
            ImGui::PopItemWidth();
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCamera(SPtr<CCamera> camera)
    {
        bool hasChanged = false;
        HCamera cameraHandle;
        cameraHandle._setHandleData(camera);
        auto& cameraSettings = camera->GetRenderSettings();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGuiExt::RenderCameraGraphics(cameraHandle, cameraSettings, width))
                hasChanged = true;
        }

        if (ImGui::CollapsingHeader("Post processing", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGuiExt::RenderCameraPostProcessing(cameraHandle, cameraSettings, width))
                hasChanged = true;
        }

        if (hasChanged)
            camera->SetRenderSettings(cameraSettings);

        return hasChanged;
    }

    bool WidgetProperties::ShowCameraFlyer(SPtr<CCameraFlyer> cameraFlyer)
    {
        bool hasChanged = false;
        Degree pitch = cameraFlyer->GetPitch();
        Degree yaw = cameraFlyer->GetYaw();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        float pitchAngle = pitch.ValueDegrees();
        float yawAngle = yaw.ValueDegrees();
        ImGuiExt::RenderOptionFloat(pitchAngle, "##camera_fliyer_pitch_option", "Current pitch", -180.0f, 180.0f, width, true);
        ImGuiExt::RenderOptionFloat(yawAngle, "##camera_fliyer_yaw_option", "Current yaw", -180.0f, 180.0f, width, true);

        return hasChanged;
    }

    bool WidgetProperties::ShowCamerUI(SPtr<CCameraUI> cameraUI)
    {
        bool hasChanged = false;
        Vector3 target = cameraUI->GetTarget();

        if (ImGuiExt::RenderVector3(target, "##camera_ui_target_option", " Target", 20.0f, false))
        {
            cameraUI->SetTarget(target);
            hasChanged = true;
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowLight(SPtr<Light> light)
    {
        bool hasChanged = false;
        float attenuationRadius = light->GetAttenuationRadius();
        float linearAttenuation = light->GetLinearAttenuation();
        float quadraticAttenuation = light->GetQuadraticAttenuation();
        float intensity = light->GetIntensity();
        Degree spotAngle = light->GetSpotAngle();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        // Color
        {
            Vector4 color = light->GetColor().GetAsVector4();
            if (ImGuiExt::RenderColorRGBA(color, "##light_color_option", "Color", width))
            {
                hasChanged = true;
                light->SetColor(Color(color));
            }
        }
        ImGui::Separator();

        // Attenuation radius
        {
            if (ImGuiExt::RenderOptionFloat(attenuationRadius, "##light_attenuation_radius_option", "Att. Radius", 0.0f, 5.0f, width))
            {
                hasChanged = true;
                light->SetAttenuationRadius(attenuationRadius);
            }
        }
        ImGui::Separator();

        // Linear attenuation
        {
            if (ImGuiExt::RenderOptionFloat(linearAttenuation, "##light_linear_attenuation_option", "Linear att.", 0.0f, 1.0f, width))
            {
                hasChanged = true;
                light->SetLinearAttenuation(linearAttenuation);
            }
        }
        ImGui::Separator();

        // quadratic attenuation
        {
            if (ImGuiExt::RenderOptionFloat(quadraticAttenuation, "##light_quadratic_attenuation_option", "Quadratic att.", 0.0f, 1.0f, width))
            {
                hasChanged = true;
                light->SetQuadraticAttenuation(quadraticAttenuation);
            }
        }
        ImGui::Separator();

        // intensity
        {
            if (ImGuiExt::RenderOptionFloat(intensity, "##light_intensity_option", "Intensity", 0.0f, 16.0f, width))
            {
                hasChanged = true;
                light->SetIntensity(intensity);
            }
        }
        ImGui::Separator();

        // Spot angle
        if (light->GetType() == LightType::Spot)
        {
            {
                float angle = spotAngle.ValueDegrees();
                if (ImGuiExt::RenderOptionFloat(angle, "##light_spot_angle_option", "Spot angle", 0.0f, 179.0f, width))
                {
                    hasChanged = true;
                    light->SetSpotAngle(Degree(angle));
                }
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowLightShadow(SPtr<Light> light)
    {
        bool hasChanged = false;
        bool castShadows = light->GetCastShadows();
        float shadowBias = light->GetShadowBias();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        // Cast shadows
        {
            if (ImGuiExt::RenderOptionBool(castShadows, "##light_cast_shadows_option", "Cast shadows"))
            {
                hasChanged = true;
                light->SetCastShadows(castShadows);
            }
        }
        ImGui::Separator();

        // Shadow bias
        {
            if (ImGuiExt::RenderOptionFloat(shadowBias, "##light_shadow_bias_option", "Shadow bias", 0.0f, 5.0f, width))
            {
                hasChanged = true;
                light->SetShadowBias(shadowBias);
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowRenderableProperties(SPtr<Renderable> renderable)
    {
        bool hasChanged = false;
        const RenderableProperties& properties = renderable->GetProperties();
        const SPtr<Mesh> mesh = renderable->GetMesh();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        ImGuiExt::ComboOptions<UUID> meshesOptions;
        UUID emptyMesh = UUID(50, 0, 0, 0);
        UUID loadMesh = UUID::EMPTY;
        UUID meshUUID = (mesh) ? mesh->GetUUID() : emptyMesh;
        EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<Mesh>();

        // current mesh to use
        {
            for (auto& resource : container.Res)
                meshesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());

            meshesOptions.AddOption(emptyMesh, ICON_FA_TIMES_CIRCLE " No mesh");
            meshesOptions.AddOption(UUID::EMPTY, ICON_FA_FOLDER_OPEN " Load");

            if (ImGuiExt::RenderOptionCombo<UUID>(&meshUUID, "##meshes_option", "Mesh", meshesOptions, width))
            {
                if (meshUUID == loadMesh)
                {
                    _loadMesh = true;
                }
                else if (meshUUID == emptyMesh)
                {
                    renderable->SetMesh(nullptr);
                    renderable->ClearAllMaterials();
                    hasChanged = true;
                }
                else if (meshUUID != ((mesh) ? mesh->GetUUID() : emptyMesh))
                {
                    renderable->SetMesh(gResourceManager().Load<Mesh>(meshUUID).GetInternalPtr());
                    renderable->ClearAllMaterials();
                    hasChanged = true;

                    // We check if a material exists on each subMesh. In this case, we apply the material on the renderable
                    for (UINT32 i = 0; i < renderable->GetMesh()->GetProperties().GetNumSubMeshes(); i++)
                    {
                        SubMesh& subMesh = renderable->GetMesh()->GetProperties().GetSubMesh(i);
                        if (subMesh.Mat.IsLoaded())
                            renderable->SetMaterial(i, subMesh.Mat.GetInternalPtr());
                    }
                }
            }

            if (ShowLoadMesh())
                hasChanged = true;
        }
        ImGui::Separator();

        // velocity
        {
            bool writeVelocity = properties.WriteVelocity;
            if (ImGuiExt::RenderOptionBool(writeVelocity, "##renderable_properties_velocity_option", "Write velocity"))
            {
                hasChanged = true;
                renderable->SetWriteVelocity(writeVelocity);
            }
        }
        ImGui::Separator();

        // instancing
        {
            bool instancing = properties.Instancing;
            if (ImGuiExt::RenderOptionBool(instancing, "##renderable_properties_instancing_option", "Instancing"))
            {
                hasChanged = true;
                renderable->SetInstancing(instancing);
            }
        }
        ImGui::Separator();

        // can be merged
        {
            bool canBeMerged = properties.CanBeMerged;
            if (ImGuiExt::RenderOptionBool(canBeMerged, "##renderable_properties_can_be_merged_option", "Merging"))
            {
                hasChanged = true;
                renderable->SetCanBeMerged(canBeMerged);
            }
        }
        ImGui::Separator();

        // cast shadows
        {
            bool castShadows = properties.CastShadows;
            if (ImGuiExt::RenderOptionBool(castShadows, "##renderable_properties_cast_shadows_option", "Cast shadows"))
            {
                hasChanged = true;
                renderable->SetCastShadows(castShadows);
            }
        }
        ImGui::Separator();

        // receive shadows
        {
            bool receiveShadows = properties.ReceiveShadows;
            if (ImGuiExt::RenderOptionBool(receiveShadows, "##renderable_properties_receive_shadows_option", "Receive shadows"))
            {
                hasChanged = true;
                renderable->SetReceiveShadows(receiveShadows);
            }
        }
        ImGui::Separator();

        // cast lights
        {
            bool castLights = properties.CastLights;
            if (ImGuiExt::RenderOptionBool(castLights, "##renderable_properties_cast_lights_option", "Cast lights"))
            {
                hasChanged = true;
                renderable->SetCastLight(castLights);
            }
        }
        ImGui::Separator();

        // use for dynamic env mapping
        {
            bool useForDynamicEnvMapping = properties.UseForDynamicEnvMapping;
            if (ImGuiExt::RenderOptionBool(useForDynamicEnvMapping, "##renderable_properties_dynamic_env_mapping_option", "Dynamic env mapping"))
            {
                hasChanged = true;
                renderable->SetUseForDynamicEnvMapping(useForDynamicEnvMapping);
            }
        }
        ImGui::Separator();

        // cull distance factor
        {
            float cullDistanceFactor = properties.CullDistanceFactor;
            if (ImGuiExt::RenderOptionFloat(cullDistanceFactor, "##renderable_properties_dynamic_env_mapping_option", "Cull dist. factor", 0.0f, 1.0f, width - 20.0f))
            {
                hasChanged = true;
                renderable->SetCullDistanceFactor(cullDistanceFactor);
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowRenderableSubMeshes(SPtr<Renderable> renderable)
    {
        bool hasChanged = false;
        SPtr<Mesh> mesh = renderable->GetMesh();
        ImGuiExt::ComboOptions<UUID> materialsOptions;
        UUID emptyMaterial = UUID(50, 0, 0, 0);
        MeshProperties& meshProperties = mesh->GetProperties();
        const float width = ImGui::GetWindowContentRegionWidth() - 120.0f;
        EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<Material>();

        for (auto& resource : container.Res)
            materialsOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());

        materialsOptions.AddOption(emptyMaterial, ICON_FA_TIMES_CIRCLE " No material");

        for (UINT32 i = 0; i < meshProperties.GetNumSubMeshes(); i++)
        {
            SPtr<Material> material = renderable->GetMaterial(i);
            UUID materialUUID = (material) ? material->GetUUID() : emptyMaterial;
            SubMesh& subMesh = meshProperties.GetSubMesh(i);
            String title = subMesh.MaterialName;
            String id = "##" + subMesh.MaterialName + ToString(i);

            // current material to use
            {
                if (ImGuiExt::RenderOptionCombo<UUID>(&materialUUID, id.c_str(), title.c_str(), materialsOptions, width))
                {
                    if (materialUUID == emptyMaterial)
                    {
                        renderable->SetMaterial(i, nullptr);
                        hasChanged = true;
                    }
                    else
                    {
                        renderable->SetMaterial(i, gResourceManager().Load<Material>(materialUUID).GetInternalPtr());
                        hasChanged = true;
                    }
                }
            }  
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowScript(SPtr<CScript> script)
    {
        bool hasChanged = false;
        Vector<String> files;
        Vector<String> directories;
        Vector<String> identifiers;
        ImGuiExt::ComboOptions<ScriptIdentifier> scriptsOptions;
        const float width = ImGui::GetWindowContentRegionWidth() - 50.0f;

        FileSystem::GetChildren(RAW_APP_ROOT + ScriptManager::LIBRARIES_PATH, files, directories, true);

        // current script to use
        {
            for (auto& file : files)
            {
                if(file.length() > 4)
                {
                    if(file.substr(file.length() - 4, 4) == ".cpp")
                    {
                        String fileName = file.substr(0, file.length() - 4);
                        ScriptIdentifier ident(fileName);

                        scriptsOptions.AddOption(ident, fileName);
                        identifiers.push_back(ident.Name);
                    }
                }
            }

            const auto& scriptLibraries = gScriptManager().GetScriptLibraries();
            for (auto& scriptLibrary : scriptLibraries)
            {
                if (std::find(identifiers.begin(), identifiers.end(), scriptLibrary.first.Name) == identifiers.end())
                {
                    scriptsOptions.AddOption(scriptLibrary.first, scriptLibrary.first.Name);
                    identifiers.push_back(scriptLibrary.first.Name);
                }
            }

            scriptsOptions.AddOption(ScriptIdentifier(""), ICON_FA_TIMES_CIRCLE " No script");
            scriptsOptions.AddOption(ScriptIdentifier("_"), ICON_FA_FOLDER_OPEN " Load");

            ScriptIdentifier previousScript(script->GetNativeScriptName(), script->GetNativeScriptPath());
            ScriptIdentifier currentScript = previousScript;

            if (ImGuiExt::RenderOptionCombo<ScriptIdentifier>(&currentScript, "##scripts_options", "Script", scriptsOptions, width))
            {
                if (currentScript.Name == "_")
                {
                    _loadScript = true;
                }
                else if (currentScript.Name != previousScript.Name)
                {
                    script->SetNativeScript(currentScript);
                    hasChanged = true;
                }
            }
        }

        if (ShowLoadScript())
            hasChanged = true;

        return hasChanged;
    }

    bool WidgetProperties::ShowAnimation(SPtr<CAnimation> animation)
    {
        bool hasChanged = false;
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        // cull
        {
            bool cull = animation->GetEnableCull();
            if (ImGuiExt::RenderOptionBool(cull, "##animation_cull_option", "Culling"))
            {
                hasChanged = true;
                animation->SetEnableCull(cull);
            }
        }
        ImGui::Separator();

        // speed
        {
            float speed = animation->GetSpeed();
            if (ImGuiExt::RenderOptionFloat(speed, "##animation_speed_option", "Speed", 0.1f, 10.0f, width))
            {
                hasChanged = true;
                animation->SetSpeed(speed);
            }
        }
        ImGui::Separator();

        // wrap mode
        {
            static ImGuiExt::ComboOptions<AnimWrapMode> wrapModeOptions;
            if (wrapModeOptions.Options.size() == 0)
            {
                wrapModeOptions.AddOption(AnimWrapMode::Loop, "Loop");
                wrapModeOptions.AddOption(AnimWrapMode::Clamp, "Clamp");
            }

            AnimWrapMode wrapMode = animation->GetWrapMode();
            if (ImGuiExt::RenderOptionCombo<AnimWrapMode>(&wrapMode, "##animation_wrap_mode_option", "Wrap mode", wrapModeOptions, width))
            {
                hasChanged = true;
                animation->SetWrapMode(wrapMode);
            }
        }
        ImGui::Separator();

        // bounds
        {
            AABox bounds = animation->GetBounds();
            Vector3 minBound = bounds.GetMin();
            Vector3 maxBound = bounds.GetMax();

            if (ImGuiExt::RenderVector3(minBound, "##animation_min_bound_option", " Min bound", 40.0f))
            {
                bounds.SetMin(minBound);
                animation->SetBounds(bounds);
                hasChanged = true;
            }

            if (ImGuiExt::RenderVector3(maxBound, "##animation_max_bound_option", " Max bound", 40.0f))
            {
                bounds.SetMax(maxBound);
                animation->SetBounds(bounds);
                hasChanged = true;
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowAnimationClips(SPtr<CAnimation> animation)
    {
        bool hasChanged = false;
        char inputName[256];
        UINT32 numClip = animation->GetNumClips();
        const float widgetWidth = ImGui::GetWindowContentRegionWidth() - 100.0f;

        for (UINT32 i = 0; i < numClip; i++)
        {
            HAnimationClip clip = animation->GetClip(i);
            String name = clip->GetName();
            
            ImGui::PushID((int)clip->GetInternalID());
            if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                memset(&inputName, 0, 256);
                if(name.length() < 256)
                    strcpy(inputName, name.c_str());
                else
                    strcpy(inputName, name.substr(0,255).c_str());

                ImGui::PushItemWidth(widgetWidth);
                if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
                {
                    clip->SetName(inputName);
                    hasChanged = true;
                }
                ImGui::PopItemWidth();
            }
            ImGui::PopID();
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowSkybox(SPtr<Skybox> skybox)
    {
        bool hasChanged = false;
        float brigtness = skybox->GetBrightness();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        ImGuiExt::ComboOptions<UUID> texturesOptions;
        UUID emptyTexture = UUID(50, 0, 0, 0);
        UUID loadTexture = UUID::EMPTY;
        UUID textureUUID = (skybox->GetTexture()) ? skybox->GetTexture()->GetUUID() : emptyTexture;
        UUID irradianceuUUID = (skybox->GetIrradiance()) ? skybox->GetIrradiance()->GetUUID() : emptyTexture;
        EditorResManager::ResourcesContainer& container = EditorResManager::Instance().Get<Texture>();

        for (auto& resource : container.Res)
        {
            SPtr<Texture> texture = std::static_pointer_cast<Texture>(resource.second.GetInternalPtr());
            if (texture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_CUBE_MAP)
                texturesOptions.AddOption(resource.second->GetUUID(), resource.second->GetName());
        }

        texturesOptions.AddOption(emptyTexture, ICON_FA_TIMES_CIRCLE " No texture");
        texturesOptions.AddOption(UUID::EMPTY, ICON_FA_FOLDER_OPEN " Load");

        // current texture to use
        {
            if (ImGuiExt::RenderOptionCombo<UUID>(&textureUUID, "##skybox_texture_option", "Texture", texturesOptions, width))
            {
                if (textureUUID == loadTexture)
                {
                    _loadSkybox = true;
                }
                else if (textureUUID == emptyTexture)
                {
                    skybox->SetTexture(nullptr);
                    hasChanged = true;
                }
                else
                {
                    skybox->SetTexture(gResourceManager().Load<Texture>(textureUUID).GetInternalPtr());
                    hasChanged = true;
                }
            }
        }
        ImGui::Separator();

        // current irradiance texture to use
        {
            if (ImGuiExt::RenderOptionCombo<UUID>(&irradianceuUUID, "##skybox_texture_irradiance_option", "Irradiance", texturesOptions, width))
            {
                if (irradianceuUUID == loadTexture)
                {
                    _loadSkyboxIrradiance = true;
                }
                else if (irradianceuUUID == emptyTexture)
                {
                    skybox->SetIrradiance(nullptr);
                    hasChanged = true;
                }
                else
                {
                    skybox->SetIrradiance(gResourceManager().Load<Texture>(irradianceuUUID).GetInternalPtr());
                    hasChanged = true;
                }
            }
        }
        ImGui::Separator();

        if (ImGuiExt::RenderOptionFloat(brigtness, "##skybox_option", "Brightness", 0.0f, 3.0f, width))
        {
            skybox->SetBrightness(brigtness);
            hasChanged = true;
        }

        if (ShowLoadSkybox())
            hasChanged = true;

        return hasChanged;
    }

    bool WidgetProperties::ShowBody(SPtr<CBody> body)
    {
        bool hasChanged = false;
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

        // Is Kinematic
        {
            bool isKinematic = body->GetIsKinematic();
            if (ImGuiExt::RenderOptionBool(isKinematic, "##body_option_is_kinematic", "Is Kinematic"))
            {
                body->SetIsKinematic(isKinematic);
                hasChanged = true;
            }
        }

        // Is Trigger
        {
            bool isTrigger = body->GetIsTrigger();
            if (ImGuiExt::RenderOptionBool(isTrigger, "##body_option_is_trigger", "Is trigger"))
            {
                body->SetIsTrigger(isTrigger);
                hasChanged = true;
            }
        }

        // Is Debug
        {
            bool isDebug = body->GetIsDebug();
            if (ImGuiExt::RenderOptionBool(isDebug, "##body_option_is_debug", "Display Debug Info"))
            {
                body->SetIsDebug(isDebug);
                hasChanged = true;
            }
        }

        // UseGravity
        {
            bool useGravity = body->GetUseGravity();
            if (ImGuiExt::RenderOptionBool(useGravity, "##body_option_use_gravity", "Use Gravity"))
            {
                body->SetUseGravity(useGravity);
                hasChanged = true;
            }
        }

        // Continuous Collision Detection (CCD)
        {
            UINT32 flags = (UINT32)body->GetFlags();
            bool ccd = flags & (UINT32)BodyFlag::CCD;
            if (ImGuiExt::RenderOptionBool(ccd, "##body_option_ccd", "Cont. Collision Detect."))
            {
                if (ccd)
                    flags |= (UINT32)BodyFlag::CCD;
                else
                    flags &= ~(UINT32)BodyFlag::CCD;

                body->SetFlags((BodyFlag)flags);
                hasChanged = true;
            }
        }

        // Collision report mode
        {
            static ImGuiExt::ComboOptions<CollisionReportMode> collisionModeOptions;

            if (collisionModeOptions.Options.size() == 0)
            {
                collisionModeOptions.AddOption(CollisionReportMode::None, "Never");
                collisionModeOptions.AddOption(CollisionReportMode::Report, "Report");
                collisionModeOptions.AddOption(CollisionReportMode::ReportPersistent, "Report continuous");
            }

            CollisionReportMode collisionMode = body->GetCollisionReportMode();
            if (ImGuiExt::RenderOptionCombo<CollisionReportMode>(&collisionMode,
                "##body_option_collision_mode", "Collision report", collisionModeOptions, width))
            {
                body->SetCollisionReportMode(collisionMode);
                hasChanged = true;
            }
        }
        ImGui::Separator();

        // Mass
        {
            float mass = body->GetMass();
            if (ImGuiExt::RenderOptionFloat(mass, "##body_option_mass", "Mass", 0.0f, 512.0f, width))
            {
                body->SetMass(mass);
                hasChanged = true;
            }
        }

        // Friction
        {
            float friction = body->GetFriction();
            if (ImGuiExt::RenderOptionFloat(friction, "##body_option_friction", "Friction", 0.0f, 8.0f, width))
            {
                body->SetFriction(friction);
                hasChanged = true;
            }
        }

        // Rolling friction
        {
            float rollingFriction = body->GetRollingFriction();
            if (ImGuiExt::RenderOptionFloat(rollingFriction, "##body_option_rolling_friction", "Rolling Friction", 0.0f, 8.0f, width))
            {
                body->SetRollingFriction(rollingFriction);
                hasChanged = true;
            }
        }

        // Restitution
        {
            float restitution = body->GetRestitution();
            if (ImGuiExt::RenderOptionFloat(restitution, "##body_option_restitution", "Restitution", 0.0f, 4.0f, width))
            {
                body->SetRestitution(restitution);
                hasChanged = true;
            }
        }
        ImGui::Separator();

        // Center of mass
        {
            Vector3 centerOfMass = body->GetCenterOfMass();
            if (ImGuiExt::RenderVector3(centerOfMass, "##body_option_center_of_mass", " Center of Mass", 60.0f))
            {
                body->SetCenterOfMass(centerOfMass);
                hasChanged = true;
            }
        }

        // Velocity
        {
            Vector3 velocity = body->GetVelocity();
            if (ImGuiExt::RenderVector3(velocity, "##body_option_velocity", " Velocity", 60.0f))
            {
                body->SetVelocity(velocity);
                hasChanged = true;
            }
        }

        // Angular velocity
        {
            Vector3 angularVelocity = body->GetAngularVelocity();
            if (ImGuiExt::RenderVector3(angularVelocity, "##body_option_angular_velocity", " Angular Velocity", 60.0f))
            {
                body->SetAngularVelocity(angularVelocity);
                hasChanged = true;
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowCollider(SPtr<CCollider> collider)
    {
        bool hasChanged = false;

        // Scale
        {
            Vector3 scale = collider->GetScale();
            if (ImGuiExt::RenderVector3(scale, "##collider_option_scale", " Scale", 20.0f))
            {
                collider->SetScale(scale);
                hasChanged = true;
            }
        }

        // Center
        {
            Vector3 center = collider->GetCenter();
            if (ImGuiExt::RenderVector3(center, "##collider_option_center", " Center", 20.0f))
            {
                collider->SetCenter(center);
                hasChanged = true;
            }
        }
        ImGui::Separator();

        // Position
        {
            Vector3 position = collider->GetPosition();
            if (ImGuiExt::RenderVector3(position, "##collider_option_position", " Position", 20.0f))
            {
                collider->SetPosition(position);
                hasChanged = true;
            }
        }

        // Rotation
        {
            Radian x, y, z;
            collider->GetRotation().ToEulerAngles(x, y, z);
            Vector3 rotation(x.ValueDegrees(), y.ValueDegrees(), z.ValueDegrees());

            if (ImGuiExt::RenderVector3(rotation, "##collider_option_rotation", " Rotation", 20.0f))
            {
                Quaternion rot;
                rot.FromEulerAngles(Radian(Degree(rotation.x)), Radian(Degree(rotation.y)), Radian(Degree(rotation.z)));
                collider->SetRotation(rot);
                hasChanged = true;
            }
        }

        return hasChanged;
    }

    bool WidgetProperties::ShowLoadMesh()
    {
        bool meshLoaded = false;

        if (_loadMesh || _loadPhysicsMesh)
            ImGui::OpenPopup("Load Mesh");

        if (_fileBrowser.ShowFileDialog("Load Mesh", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, ".obj,.dae,.fbx,.stl,.gltf"))
        {
            auto meshImportOptions = MeshImportOptions::Create();
            meshImportOptions->ImportNormals = _fileBrowser.Data.MeshParam.ImportNormals;
            meshImportOptions->ImportTangents = _fileBrowser.Data.MeshParam.ImportTangents;
            meshImportOptions->ImportSkin = _fileBrowser.Data.MeshParam.ImportSkin;
            meshImportOptions->ImportBlendShapes = _fileBrowser.Data.MeshParam.ImportBlendShapes;
            meshImportOptions->ImportAnimations = _fileBrowser.Data.MeshParam.ImportAnimations;
            meshImportOptions->ReduceKeyFrames = _fileBrowser.Data.MeshParam.ReduceKeyFrames;
            meshImportOptions->ImportMaterials = _fileBrowser.Data.MeshParam.ImportMaterials;
            meshImportOptions->ImportVertexColors = _fileBrowser.Data.MeshParam.ImportVertexColors;
            meshImportOptions->ForceGenNormals = _fileBrowser.Data.MeshParam.ForceGenNormals;
            meshImportOptions->GenSmoothNormals = _fileBrowser.Data.MeshParam.GenSmoothNormals;
            meshImportOptions->ScaleSystemUnit = _fileBrowser.Data.MeshParam.ScaleSystemUnit;
            meshImportOptions->ScaleFactor = _fileBrowser.Data.MeshParam.ScaleFactor;
            meshImportOptions->CollisionShape = _fileBrowser.Data.MeshParam.CollisionShape;
            meshImportOptions->CpuCached = false;

            SPtr<MultiResource> resources = EditorResManager::Instance().LoadAll(_fileBrowser.Data.SelectedPath, meshImportOptions);
            if (!resources->Empty())
            {
                for (auto& subRes : resources->Entries)
                {
                    if (subRes.Name == "primary")
                    {
                        HMesh mesh = static_resource_cast<Mesh>(subRes.Res);
                        if (mesh.IsLoaded())
                        {
                            mesh->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                            EditorResManager::Instance().Add<Mesh>(mesh);

                            if (_selections.ClickedComponent->GetCoreType() == TID_CRenderable)
                            {
                                SPtr<CRenderable> renderable = std::static_pointer_cast<CRenderable>(_selections.ClickedComponent);

                                // We will try to set the material attach to this mesh (in fact one material per submesh), and create it before if not exist
                                renderable->SetMesh(mesh.GetInternalPtr());
                                meshLoaded = true;

                                if (meshImportOptions->ImportMaterials)
                                {
                                    EditorUtils::ImportMeshMaterials(mesh);

                                    for (UINT32 i = 0; i < mesh->GetProperties().GetNumSubMeshes(); i++)
                                    {
                                        SubMesh& subMesh = mesh->GetProperties().GetSubMesh(i);
                                        if (subMesh.Mat.IsLoaded())
                                            renderable->SetMaterial(i, subMesh.Mat.GetInternalPtr());
                                    }
                                }
                            }
                        }

                        _loadMesh = false;
                        _loadPhysicsMesh = false;
                    }
                    else if (subRes.Name == "collision")
                    {
                        HPhysicsMesh physicsMesh = static_resource_cast<PhysicsMesh>(subRes.Res);
                        if (physicsMesh.IsLoaded())
                        {
                            EditorResManager::Instance().Add<PhysicsMesh>(physicsMesh);

                            if (_selections.ClickedComponent->GetCoreType() == TID_CMeshCollider)
                            {
                                SPtr<CMeshCollider> meshCollider = std::static_pointer_cast<CMeshCollider>(_selections.ClickedComponent);

                                meshCollider->SetMesh(physicsMesh);
                                meshLoaded = true;
                            }
                        }
                    }
                    else
                    {
                        subRes.Res->SetPath(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                    }
                }
            }
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
            {
                _loadMesh = false;
                _loadPhysicsMesh = false;
            }
        }

        return meshLoaded;
    }

    bool WidgetProperties::ShowLoadSkybox()
    {
        bool skyboxLoaded = false;

        if (_loadSkybox || _loadSkyboxIrradiance)
            ImGui::OpenPopup("Load Skybox Texture");

        if (_fileBrowser.ShowFileDialog("Load Skybox Texture", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), false, ".png,.jpeg,.jpg"))
        {
            auto textureSkyboxImportOptions = TextureImportOptions::Create();
            textureSkyboxImportOptions->CubemapType = CubemapSourceType::Faces;
            textureSkyboxImportOptions->IsCubemap = true;
            textureSkyboxImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
            textureSkyboxImportOptions->CpuCached = _fileBrowser.Data.TexParam.CpuCached;

            HTexture texture = EditorResManager::Instance().Load<Texture>(_fileBrowser.Data.SelectedPath, textureSkyboxImportOptions);
            if (texture.IsLoaded())
            {
                texture->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                EditorResManager::Instance().Add<Texture>(texture);
                SPtr<CSkybox> skybox = std::static_pointer_cast<CSkybox>(_selections.ClickedComponent);

                if(_loadSkybox)
                    skybox->SetTexture(texture.GetInternalPtr());
                else
                    skybox->SetIrradiance(texture.GetInternalPtr());

                skyboxLoaded = true;
            }

            _loadSkybox = false;
            _loadSkyboxIrradiance = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _loadSkybox = false;
        }

        return skyboxLoaded;
    }

    bool WidgetProperties::ShowLoadScript()
    {
        bool scriptLoaded = false;

        if (_loadScript)
            ImGui::OpenPopup("Load Script");

        if (_fileBrowser.ShowFileDialog("Load Script", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), false, ".cpp"))
        {
            if (_selections.ClickedComponent && _selections.ClickedComponent->GetCoreType() == TID_CScript)
            {
                String fileName = _fileBrowser.Data.SelectedFileName;
                fileName = fileName.substr(0, fileName.length() - 4);
                SPtr<CScript> script = std::static_pointer_cast<CScript>(_selections.ClickedComponent);
                script->SetNativeScript(fileName, _fileBrowser.Data.SelectedPath);
            }

            _loadScript = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _loadScript = false;
        }

        return scriptLoaded;
    }

    bool WidgetProperties::ShowLoadAudioClip()
    {
        bool audioScriptLoaded = false;

        if (_loadAudioClip)
            ImGui::OpenPopup("Load Audio Clip");

        if (_fileBrowser.ShowFileDialog("Load Audio Clip", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, ".ogg,.flac,.wav"))
        {
            auto audioClipImportOptions = AudioClipImportOptions::Create();
            audioClipImportOptions->Is3D = _fileBrowser.Data.AudioParam.Is3D;

            HAudioClip audioClip = EditorResManager::Instance().Load<AudioClip>(_fileBrowser.Data.SelectedPath, audioClipImportOptions);
            if (audioClip.IsLoaded())
            {
                audioClip->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                EditorResManager::Instance().Add<AudioClip>(audioClip);

                SPtr<CAudioSource> audioSource = std::static_pointer_cast<CAudioSource>(_selections.ClickedComponent);
                audioSource->SetClip(audioClip);
                audioScriptLoaded = true;
            }

            _loadAudioClip = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _loadAudioClip = false;
        }
        
        return audioScriptLoaded;
    }

    bool WidgetProperties::ShowLoadHeightFieldTexture()
    {
        bool heightFieldTextureLoaded = false;

        if (_loadHeightFieldTexture)
            ImGui::OpenPopup("Load Height Field Texture");

        if (_fileBrowser.ShowFileDialog("Load Height Field Texture", ImGuiFileBrowser::DialogMode::OPEN, ImVec2(900, 450), true, ".png,.jpeg,.jpg"))
        {
            auto textureImportOptions = TextureImportOptions::Create();
            if (_fileBrowser.Data.TexParam.TexType != TextureType::TEX_TYPE_CUBE_MAP)
            {
                textureImportOptions->CpuCached = _fileBrowser.Data.TexParam.CpuCached;
                textureImportOptions->GenerateMips = _fileBrowser.Data.TexParam.GenerateMips;
                textureImportOptions->MaxMip = _fileBrowser.Data.TexParam.MaxMips;
                textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

                HTexture texture = EditorResManager::Instance().Load<Texture>(_fileBrowser.Data.SelectedPath, textureImportOptions, true);
                if (texture.IsLoaded())
                {
                    texture->SetName(UTF8::FromANSI(_fileBrowser.Data.SelectedFileName));
                    EditorResManager::Instance().Add<Texture>(texture);

                    if (texture->GetProperties().GetTextureType() != TextureType::TEX_TYPE_CUBE_MAP)
                    {
                        SPtr<CHeightFieldCollider> heightFieldCollider = std::static_pointer_cast<CHeightFieldCollider>(_selections.ClickedComponent);
                        heightFieldCollider->SetHeightField(GetOrCreatePhysicsHightFieldFromTex(texture.GetInternalPtr()));
                    }
                }
            }

            _loadHeightFieldTexture = false;
        }
        else
        {
            if (_fileBrowser.Data.IsCancelled)
                _loadHeightFieldTexture = false;
        }

        return heightFieldTextureLoaded;
    }

    HPhysicsHeightField WidgetProperties::GetOrCreatePhysicsHightFieldFromTex(SPtr<Texture> texture)
    {
        if (!texture)
            return HPhysicsHeightField();

        EditorResManager::ResourcesContainer& HeightFieldContainer = EditorResManager::Instance().Get<PhysicsHeightField>();

        for (auto& resource : HeightFieldContainer.Res)
        {
            HPhysicsHeightField heightField = static_resource_cast<PhysicsHeightField>(resource.second);
            
            if (heightField->GetTexture() == texture)
                return heightField;
        }

        HPhysicsHeightField physicsHeightField = PhysicsHeightField::Create(texture);
        if (physicsHeightField.IsLoaded())
            EditorResManager::Instance().Add<PhysicsHeightField>(physicsHeightField);

        return physicsHeightField;
    }
}
