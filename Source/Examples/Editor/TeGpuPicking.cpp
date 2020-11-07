#include "TeGpuPicking.h"

#include "TeEditor.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Resources/TeBuiltinResources.h"

namespace te
{
    void GpuPicking::Initialize()
    { 
        _shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::GpuPicking);
        _perCameraParamBuffer = _perCameraParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();
    }

    void GpuPicking::ComputePicking(const HCamera& camera, const RenderParam& param, const HSceneObject& root)
    { 
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;

        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();
        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;

        _perCameraParamDef.gMatViewProj.Set(_perCameraParamBuffer, viewProjMatrix.Transpose());

        CheckRenderTexture(param.Width, param.Height);

        rapi.SetRenderTarget(_renderData.RenderTex);
        rapi.ClearViewport(clearBuffers, Color::Blue);

        // TODO draw

        rapi.SetRenderTarget(nullptr);
    }

    Color GpuPicking::GetColorAt(UINT32 x, UINT32 y)
    { 
        SPtr<Texture> pickingTexture = (static_cast<RenderTexture&>(*(_renderData.RenderTex))).GetColorTexture(0);
        SPtr<PixelData> pixelData = pickingTexture->GetProperties().AllocBuffer(0, 0);
        pickingTexture->ReadData(*pixelData);

        return pixelData->GetColorAt(x, y);
    }

    bool GpuPicking::CheckRenderTexture(UINT32 width, UINT32 height)
    {
        if (width == _renderData.Width && height == _renderData.Height)
            return false;

        _renderData.Width = width;
        _renderData.Height = height;

        EditorUtils::GenerateViewportRenderTexture(_renderData);

        return true;
    }
}
