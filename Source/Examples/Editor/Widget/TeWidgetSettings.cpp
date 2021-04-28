#include "TeWidgetSettings.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Physics/TePhysics.h"

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
        const float width = ImGui::GetWindowContentRegionWidth() - 75.0f;

        if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Physic paused
            {
                bool physicsPaused = !gPhysics().IsPaused();
                if (ImGuiExt::RenderOptionBool(physicsPaused, "##settings_physics_enable", "Enable Physic"))
                    gPhysics().SetPaused(!physicsPaused);
            }

            // Debug paused
            {
                bool physicsDebug = gPhysics().IsDebug();
                if (ImGuiExt::RenderOptionBool(physicsDebug, "##settings_physics_enable", "Enable Physic Debug"))
                    gPhysics().SetDebug(physicsDebug);
            }
            ImGui::Separator();
            
            // Gravity
            {
                Vector3 gravity = gPhysics().GetDesc().Gravity;
                if (ImGuiExt::RenderVector3(gravity, "##settings_physics_gravity", " Gravity", width))
                    gPhysics().SetGravity(gravity);
            }
        }
    }

    void WidgetSettings::UpdateBackground()
    { }
}