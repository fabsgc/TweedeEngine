#include "TeGpuPicking.h"

#include "TeEditor.h"
#include "Resources/TeBuiltinResources.h"

namespace te
{
    GpuPicking::GpuPicking()
    { }

    GpuPicking::~GpuPicking()
    { }

    void GpuPicking::Initialize()
    { 
        _shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::GpuPicking);
        _perCameraParamBuffer = _perCameraParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();
    }

    void GpuPicking::ComputePicking(const HCamera& camera, const RenderParam& param, const HSceneObject& root)
    { }

    Color GpuPicking::GetColorAt(UINT32 x, UINT32 y)
    { 
        return Color();
    }

    bool GpuPicking::CheckRenderTexture(const float& width, const float& height)
    {
        return false;
    }
}
