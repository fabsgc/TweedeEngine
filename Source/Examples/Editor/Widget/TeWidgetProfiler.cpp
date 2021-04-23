#include "TeWidgetProfiler.h"

#include "Math/TeMath.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Profiling/TeProfilerGPU.h"

namespace te
{
    WidgetProfiler::WidgetProfiler()
        : Widget(WidgetType::Profiler)
    { 
        _title = PROFILER_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetProfiler::~WidgetProfiler()
    { }

    void WidgetProfiler::Initialize()
    { }

    void WidgetProfiler::Update()
    {
        bool profilerEnabled = gProfilerGPU().IsEnabled();
        if (ImGuiExt::RenderOptionBool(profilerEnabled, "##profiler_gpu_enabled_option", "Enable GPU profiling"))
        {
            gProfilerGPU().Enable(profilerEnabled);
        }
        ImGui::Separator();

        const GPUSample& sample = gProfilerGPU().GetSample();

        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            String frameTime = ToString((float)sample.Time / 1000.0f) + " ms";

            ImGui::PushID("Renderer Profiling ID");
            {
                ImGui::BeginChild("Renderer Profiling Fields", ImVec2(ImGui::GetContentRegionAvail().x, 418.0f), true);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5.0f, 5.0f });

                ImGui::Columns(2);

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Frame time");
                ImGui::NextColumn();
                ImGui::Text(frameTime.c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Draw calls");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumDrawCalls).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Compute calls");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumComputeCalls).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Presents");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumPresents).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Clears");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumClears).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Render Target Chg.");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumRenderTargetChanges).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Pipeline state Chg.");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumPipelineStateChanges).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Vertices");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumVertices).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Primitives");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumPrimitives).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Instances");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumInstances).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("GPU Param Binds");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumGpuParamBinds).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Vertex Buffer Binds");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumVertexBufferBinds).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Index Buffer Binds");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumIndexBufferBinds).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Num Res Created");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumObjectsCreated).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Num Res Read");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumResourceReads).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Num Res Write");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumResourceWrites).c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Num Res Destroyed");
                ImGui::NextColumn();
                ImGui::Text(ToString(sample.NumObjectsDestroyed).c_str());
                ImGui::NextColumn();

                ImGui::PopStyleVar();
                ImGui::EndChild();
            }
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("GPU Memory", ImGuiTreeNodeFlags_DefaultOpen))
        {
            String GPUMemory = ToString(sample.GPUMemory) + " MB";
            String UsedGPUMemory = ToString(sample.UsedGPUMemory) + " MB";

            ImGui::PushID("GPU Memory ID");
            {
                ImGui::BeginChild("GPU Memory Fields", ImVec2(ImGui::GetContentRegionAvail().x, 58.0f), true);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5.0f, 5.0f });

                ImGui::Columns(2);

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("GPU Mem.");
                ImGui::NextColumn();
                ImGui::Text(GPUMemory.c_str());
                ImGui::NextColumn();

                ImGui::Separator();

                ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() - 75.0f);
                ImGui::Text("Used GPU Mem.");
                ImGui::NextColumn();
                ImGui::Text(UsedGPUMemory.c_str());
                ImGui::NextColumn();

                ImGui::Columns(1);

                ImGui::PopStyleVar();
                ImGui::EndChild();
            }
            ImGui::PopID();
        }
    }

    void WidgetProfiler::UpdateBackground()
    { }
}
