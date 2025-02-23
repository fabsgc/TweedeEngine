#include "TeWidgetSettings.h"

#include "../TeEditor.h"
#include "Physics/TePhysics.h"
#include "Animation/TeAnimationManager.h"
#include "Audio/TeAudio.h"

namespace te
{
    WidgetSettings::WidgetSettings()
        : Widget(WidgetType::Settings)
    { 
        _title = SETTINGS_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetSettings::~WidgetSettings()
    { }

    void WidgetSettings::Initialize()
    { }

    void WidgetSettings::Update()
    {
        const float width = ImGui::GetWindowContentRegionWidth() - 110.0f;

        if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool physicsStarted = !gPhysics().IsPaused();

            // Physic paused
            {
                if (ImGuiExt::RenderOptionBool(physicsStarted, "##settings_physics_enable", "Enable Physics"))
                {
                    gPhysics().SetPaused(!physicsStarted);
                    gEditor().NeedsRedraw();
                }
            }

            if (physicsStarted)
            {
                // Debug paused
                {
                    bool physicsDebug = gPhysics().IsDebug();
                    if (ImGuiExt::RenderOptionBool(physicsDebug, "##settings_physics_enable", "Enable Physic Debug"))
                    {
                        gPhysics().SetDebug(physicsDebug);
                        gEditor().NeedsRedraw();
                    }
                }
                ImGui::Separator();
                
                // Gravity
                {
                    Vector3 gravity = gPhysics().GetDesc().Gravity;
                    if (ImGuiExt::RenderVector3(gravity, "##settings_physics_gravity", " Gravity", width))
                    {
                        gPhysics().SetGravity(gravity);
                        gEditor().NeedsRedraw();
                    }
                }
                ImGui::Separator();

                // Water Normal
                {
                    Vector3 waterNormal = gPhysics().GetDesc().WaterNormal;
                    if (ImGuiExt::RenderVector3(waterNormal, "##settings_physics_water_normal", " Water Normal", width))
                    {
                        gPhysics().SetWaterNormal(waterNormal);
                        gEditor().NeedsRedraw();
                    }
                }
                ImGui::Separator();

                // Air Density
                {
                    float airDensity = gPhysics().GetDesc().AirDensity;
                    if (ImGuiExt::RenderOptionFloat(airDensity, "##settings_physics_air_density", " Air Density", 0.0f, 32.0f, width))
                    {
                        gPhysics().SetAirDensity(airDensity);
                        gEditor().NeedsRedraw();
                    }
                }

                // Water Density
                {
                    float waterDensity = gPhysics().GetDesc().WaterDensity;
                    if (ImGuiExt::RenderOptionFloat(waterDensity, "##settings_physics_water_density", " Water Density", 0.0f, 32.0f, width))
                    {
                        gPhysics().SetWaterDensity(waterDensity);
                        gEditor().NeedsRedraw();
                    }
                }

                // Water Offset
                {
                    float waterOffset = gPhysics().GetDesc().WaterOffset;
                    if (ImGuiExt::RenderOptionFloat(waterOffset, "##settings_physics_water_offset", " Water Offset", 0.0f, 32.0f, width))
                    {
                        gPhysics().SetWaterOffset(waterOffset);
                        gEditor().NeedsRedraw();
                    }
                }
            }
        }

        if (ImGui::CollapsingHeader("Animations", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool animationsStarted = !gAnimationManager().IsPaused();

            // Animations paused
            {
                bool animationsStarted = !gAnimationManager().IsPaused();
                if (ImGuiExt::RenderOptionBool(animationsStarted, "##settings_animations_enable", "Enable Animations"))
                {
                    gAnimationManager().SetPaused(!animationsStarted);
                    gEditor().NeedsRedraw();
                }
            }

            if (animationsStarted)
            {
                // Debug paused
                {
                    bool animationDebug = gEditor().IsAnimationDebug();
                    if (ImGuiExt::RenderOptionBool(animationDebug, "##settings_physics_enable", "Enable Animations Debug"))
                    {
                        gEditor().SetAnimationDebug(animationDebug);
                        gEditor().NeedsRedraw();
                    }
                }
            }
        }

        if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool running = !gAudio().IsPaused();

            // Audio pause
            {
                if (ImGuiExt::RenderOptionBool(running, "##settings_audio_enable", "Enable Audio"))
                {
                    gAudio().SetPaused(!running);
                }
            }

            if (running)
            {
                // Device
                {
                    AudioDevice activeDevice = gAudio().GetActiveDevice();
                    ImGuiExt::ComboOptions<AudioDevice> options;

                    const Vector<AudioDevice>& devices = gAudio().GetAllDevices();

                    options.AddOption(AudioDevice(), "Default");
                    for (const auto device : devices)
                    {
                        if (device.Name != "")
                            options.AddOption(device, device.Name != "" ? device.Name : "Default");
                    }

                    if (ImGuiExt::RenderOptionCombo<AudioDevice>(&activeDevice, "##settings_audio_device", "Audio output", options, width))
                    {
                        gAudio().SetActiveDevice(activeDevice);
                    }
                }

                // Volume
                {
                    float volume = gAudio().GetVolume();
                    if (ImGuiExt::RenderOptionFloat(volume, "##settings_audio_volume", "Volume", 0.f, 1.f, width))
                    {
                        gAudio().SetVolume(volume);
                    }
                }
            }
        }

        if (ImGui::CollapsingHeader("Editor", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool displayBoundaries = gEditor().GetDisplayBoundaries();
            if (ImGuiExt::RenderOptionBool(displayBoundaries, "##settings_editor_boundaries", "Display bouding boxes of selected object"))
            {
                gEditor().SetDisplayBoundaries(displayBoundaries);
                gEditor().NeedsRedraw();
            }
        }
    }

    void WidgetSettings::UpdateBackground()
    { }
}
