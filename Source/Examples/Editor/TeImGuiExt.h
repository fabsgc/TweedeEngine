#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"
#include "Scene/TeTransform.h"

namespace te
{
    class ImGuiExt
    {
    public:
        static bool RenderOptionFloat(float& value, const char* id, const char* text, float min = 0.0f, 
            float max = std::numeric_limits<float>::max(), float width = 0.0f);

        static bool RenderOptionInt(int& value, const char* id, const char* text, int min = 0, 
            int max = std::numeric_limits<int>::max(), float width = 0.0f);

        static bool RenderOptionBool(bool& value, const char* id, const char* text);

        static bool RenderOptionCombo(int* value, const char* id, const char* text,
            Vector<int>& options, Vector<String>& labels, float width = 0.0f);

        static bool RenderVector3(Vector3& vector, const char* id, const char* text, float width = 0.0f);

        static bool RenderTransform(Transform& transform, const char* text);
    };
}