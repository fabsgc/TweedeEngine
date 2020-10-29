#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Scene/TeTransform.h"

namespace te
{
    class ImGuiExt
    {
    public:
        struct ComboOption
        {
            int Key;
            String Label;

            ComboOption(int key, const String& label)
                : Key(key)
                , Label(label)
            { }
        };

        struct ComboOptions
        {
            Vector<ComboOption> Options;

            ComboOptions()
            { }

            void AddOption(int key, const String& label)
            {
                Options.push_back(ComboOption(key, label));
            }

            void AddOption(const ComboOption& option)
            {
                Options.push_back(option);
            }

            const ComboOption& operator[](int key)
            {
                for (const auto& option : Options)
                {
                    if (option.Key == key)
                        return option;
                }

                assert(false);
                return Options[0];
            }
        };

    public:
        static bool RenderOptionFloat(float& value, const char* id, const char* text, float min = 0.0f, 
            float max = std::numeric_limits<float>::max(), float width = 0.0f, bool disable = false);

        static bool RenderOptionInt(int& value, const char* id, const char* text, int min = 0, 
            int max = std::numeric_limits<int>::max(), float width = 0.0f, bool disable = false);

        static bool RenderOptionBool(bool& value, const char* id, const char* text, 
            bool disable = false);

        static bool RenderOptionCombo(int* value, const char* id, const char* text, ComboOptions& options, 
            float width = 0.0f, bool disable = false);

        static bool RenderVector3(Vector3& vector, const char* id, const char* text, float width = 0.0f, 
            bool disable = false);

        static bool RenderTransform(Transform& transform, const char* text, 
            bool disable = false);

        static bool RenderColorRGBA(Vector4& color, const char* id, const char* text, float width = 0.0f,
            bool disable = false);

        static bool RenderCameraGraphics(HCamera& camera, SPtr<RenderSettings> cameraSettings, float width = 0.0f);
        static bool RenderCameraPostProcessing(HCamera& camera, SPtr<RenderSettings> cameraSettings, float width = 0.0f);
    };
}
