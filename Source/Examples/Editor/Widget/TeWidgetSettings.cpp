#include "TeWidgetSettings.h"

#include "../TeEditor.h"
#include "Physics/TePhysics.h"
#include "Animation/TeAnimationManager.h"

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
            // Physic paused
            {
                bool physicsStarted = !gPhysics().IsPaused();
                if (ImGuiExt::RenderOptionBool(physicsStarted, "##settings_physics_enable", "Enable Physic"))
                {
                    gPhysics().SetPaused(!physicsStarted);
                    gEditor().NeedsRedraw();
                }
            }

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

        if (ImGui::CollapsingHeader("Animations", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Animations paused
            {
                bool animationsStarted = !gAnimationManager().IsPaused();
                if (ImGuiExt::RenderOptionBool(animationsStarted, "##settings_animations_enable", "Enable Animations"))
                {
                    gAnimationManager().SetPaused(!animationsStarted);
                    gEditor().NeedsRedraw();
                }
            }

            // Debug paused
            {
                bool animationDebug = gEditor().IsAnimationDebug();
                if (ImGuiExt::RenderOptionBool(animationDebug, "##settings_physics_enable", "Enable Animations Debug"))
                {
                    gEditor().SetAnimationDebug(animationDebug);
                    gEditor().NeedsRedraw();
                }
            }
            ImGui::Separator();
        }
    }

    void WidgetSettings::UpdateBackground()
    { }
}
