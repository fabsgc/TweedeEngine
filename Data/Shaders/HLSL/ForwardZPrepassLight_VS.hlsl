#include "Include/Forward.hlsli"
#include "Include/Forward_VS.hlsli"
#include "Include/Skinning.hlsli"

VS_Z_OUTPUT main( VS_Z_INPUT IN, uint instanceid : SV_InstanceID )
{
    VS_Z_OUTPUT OUT = (VS_Z_OUTPUT)0;

    float3x4 blendMatrix = (float3x4)0;
    float3x4 prevBlendMatrix = (float3x4)0;

    if(instanceid == 0)
    {
        if(gHasAnimation)
        {
            blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
            prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        }

        OUT.Position = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.Position = float4(mul(blendMatrix, OUT.Position), 1.0);
        OUT.Position = mul(gMatWorld, OUT.Position);
        OUT.Position = mul(gCamera.MatViewProj, OUT.Position);
    }
    else
    {
        if(gInstanceData[instanceid].HasAnimation)
        {
            blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
            prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        }

        OUT.Position = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.Position = float4(mul(blendMatrix, OUT.Position), 1.0);
        OUT.Position = mul(gInstanceData[instanceid].MatWorld, OUT.Position);
        OUT.Position = mul(gCamera.MatViewProj, OUT.Position);
    }

    return OUT;
}
