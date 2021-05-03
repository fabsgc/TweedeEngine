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
        const float width = ImGui::GetWindowContentRegionWidth() - 75.0f;

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
        }
    }

    void WidgetSettings::UpdateBackground()
    { }
}
