#include "TeWidgetResources.h"

#include "../TeEditor.h"
#include "../TeEditorResManager.h"
#include "Mesh/TeMesh.h"
#include "Image/TeTexture.h"
#include "Animation/TeAnimationClip.h"
#include "Audio/TeAudioClip.h"

namespace te
{
    WidgetResources::WidgetResources()
        : Widget(WidgetType::Resources)
    { 
        _title = RESOURCES_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetResources::~WidgetResources()
    { }

    void WidgetResources::Initialize()
    { }

    void WidgetResources::Update()
    {
        EditorResManager::ResourcesContainer& textureContainer = EditorResManager::Instance().Get<Texture>();
        EditorResManager::ResourcesContainer& meshContainer = EditorResManager::Instance().Get<Mesh>();
        EditorResManager::ResourcesContainer& animationClipContainer = EditorResManager::Instance().Get<AnimationClip>();
        EditorResManager::ResourcesContainer& audioClipContainer = EditorResManager::Instance().Get<AudioClip>();

        char inputPath[256];
        char inputUUID[64];
        char inputName[256];

        {
            ImGui::BeginGroup();
            ImGui::BeginChild("Textures", ImVec2(ImGui::GetWindowContentRegionWidth() / 2.0f, 0.0f), false);
            if (ImGui::CollapsingHeader("   Textures", ImGuiTreeNodeFlags_Leaf))
            {
                ImGui::BeginChild("TexturesList", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() - 35.0f), true);

                for (auto& resource : textureContainer.Res)
                {
                    ImGui::PushID((int)resource.second->GetInternalID());
                    {
                        memset(inputPath, '\0', 256 * sizeof(char));
                        memset(inputUUID, '\0', 64 * sizeof(char));
                        memset(inputName, '\0', 128 * sizeof(char));

                        strcpy(inputPath, resource.second->GetPath().c_str());
                        strcpy(inputUUID, resource.second->GetUUID().ToString().c_str());
                        strcpy(inputName, resource.second->GetName().c_str());

                        ImGui::BeginChild("TexturePreview", ImVec2(96.0f, 96.0f), true, ImGuiWindowFlags_NoScrollbar);

                        SPtr<Texture> texture = std::static_pointer_cast<Texture>(resource.second.GetInternalPtr());

                        if (texture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_2D)
                        {
                            SPtr<TextureView> textureView = texture->RequestView(
                                0, texture->GetProperties().GetNumMipmaps(),
                                0, texture->GetProperties().GetNumFaces(),
                                GVU_DEFAULT
                            );

                            void* rawData = textureView->GetRawData();

                            ImGui::Image(
                                static_cast<ImTextureID>(rawData),
                                ImVec2(static_cast<float>(80.0f), static_cast<float>(80.0f)),
                                ImVec2(0, 0),
                                ImVec2(1, 1)
                            );
                        }
                        else
                        {
                            ImVec4 color(0.2f, 0.4f, 1.0f, 1.0f);
                            ImDrawList* drawList = ImGui::GetWindowDrawList();
                            ImVec2 rectSize = ImGui::GetCursorScreenPos();
                            rectSize.x += 80.0f; rectSize.y += 80.0f;
                            drawList->AddRectFilled(ImGui::GetCursorScreenPos(), rectSize, ImU32(ImColor(color)), 0, 0);
                        }

                        ImGui::EndChild();
                        ImGui::SameLine();

                        ImGui::BeginChild("TexturesFields", ImVec2(ImGui::GetContentRegionAvail().x, 96.0f), true);
                        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 50.0f);
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 1.0f });
                        ImGui::InputText("Path", inputPath, IM_ARRAYSIZE(inputPath));
                        ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
                        ImGui::PopStyleVar();

                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();

                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 1.0f });
                        if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
                            resource.second->SetName(inputName);
                        ImGui::PopStyleVar();
                        ImGui::PopItemWidth();
                        ImGui::EndChild();
                    }
                    ImGui::PopID();
                }

                ImGui::EndChild();
            }
            ImGui::EndChild();
            ImGui::EndGroup();
        }
        ImGui::SameLine();

        auto ShowDefaultResourceField = [&](HResource& resource)
        {
            ImGui::PushID((int)resource->GetInternalID());
            ImGui::BeginChild("MeshField", ImVec2(ImGui::GetContentRegionAvail().x, 96.0f), true);
            {
                memset(inputPath, '\0', 256 * sizeof(char));
                memset(inputUUID, '\0', 64 * sizeof(char));
                memset(inputName, '\0', 128 * sizeof(char));

                strcpy(inputPath, resource->GetPath().c_str());
                strcpy(inputUUID, resource->GetUUID().ToString().c_str());
                strcpy(inputName, resource->GetName().c_str());

                ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 50.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 1.0f });
                ImGui::InputText("Path", inputPath, IM_ARRAYSIZE(inputPath));
                ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
                ImGui::PopStyleVar();

                ImGui::PopItemFlag();
                ImGui::PopStyleVar();

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 1.0f });
                if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
                    resource->SetName(inputName);
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
            }
            ImGui::EndChild();
            ImGui::PopID();
        };

        auto ShowResourceType = [&](
            EditorResManager::ResourcesContainer& container, 
            const char* groupTitle, 
            const char* headerTtitle,
            const char* listTitle,
            ImVec2 size)
        {
            ImGui::BeginGroup();
            ImGui::BeginChild(groupTitle, size, false);
            if (ImGui::CollapsingHeader(headerTtitle, ImGuiTreeNodeFlags_Leaf))
            {
                ImGui::BeginChild(listTitle, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() - 35.0f), true);

                for (auto& resource : container.Res)
                    ShowDefaultResourceField(resource.second);

                ImGui::EndChild();
            }
            ImGui::EndChild();
            ImGui::EndGroup();
        };

        ShowResourceType(meshContainer, "Meshes", "   Meshes", "MeshesList", ImVec2(0.0f, 0.0f));
        ImGui::SameLine();

        if (animationClipContainer.Res.size() > 0)
        {
            ShowResourceType(animationClipContainer, "AnimationClips", "   Animation Clips", "AnimationClipsList", 
                ImVec2(ImGui::GetWindowContentRegionWidth() / 2.0f, 0.0f));
            ImGui::SameLine();
        }
        
        if (audioClipContainer.Res.size() > 0)
        {
            ShowResourceType(audioClipContainer, "AudioClips", "   Audio Clips", "AudioClipsList",
                ImVec2(ImGui::GetWindowContentRegionWidth() / 2.0f, 0.0f));
            ImGui::SameLine();
        }
    }

    void WidgetResources::UpdateBackground()
    { }
}
