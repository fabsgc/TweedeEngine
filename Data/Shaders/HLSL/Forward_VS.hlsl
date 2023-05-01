#include "Include/Forward.hlsli"
#include "Include/Forward_VS.hlsli"
#include "Include/Skinning.hlsli"

// WRITE_VELOCITY (false, true)
// SKINNED (false, true)

VS_OUTPUT main( VS_INPUT IN, uint instanceid : SV_InstanceID )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

#if SKINNED == 1
    float3x4 blendMatrix = (float3x4)0;
    float3x4 prevBlendMatrix = (float3x4)0;
#endif

    if(instanceid == 0)
    {
        OUT.Position = float4(IN.Position, 1.0f);
        OUT.CurrPosition = float4(IN.Position, 1.0f); // Clip Space
        OUT.PrevPosition = float4(IN.Position, 1.0f); // Clip Space
        OUT.PositionWS = float4(IN.Position, 1.0f); // World Space

        OUT.Normal = IN.Normal;
        OUT.Tangent = IN.Tangent;
        OUT.BiTangent = IN.BiTangent;

#if SKINNED == 1
        if(gHasAnimation)
        {
            blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
            prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);

            OUT.Position = float4(mul(blendMatrix, OUT.Position), 1.0);
            OUT.CurrPosition = float4(mul(blendMatrix, OUT.CurrPosition), 1.0);
            OUT.PrevPosition = float4(mul(prevBlendMatrix, OUT.PrevPosition), 1.0);

            OUT.Normal = mul(blendMatrix, float4(OUT.Normal, 0.0f)).xyz;
            OUT.Tangent = float4(mul(blendMatrix, OUT.Tangent), 0.0f);
            OUT.BiTangent = float4(mul(blendMatrix, OUT.BiTangent), 0.0f);

            OUT.PositionWS = float4(mul(blendMatrix, OUT.PositionWS), 1.0);
        }
#endif

        OUT.Position = mul(gMatWorld, OUT.Position);
        OUT.Position = mul(gCamera.MatViewProj, OUT.Position);

        OUT.CurrPosition = mul(gMatWorld, OUT.CurrPosition);
        OUT.CurrPosition = mul(gCamera.MatViewProj, OUT.CurrPosition);

        OUT.PrevPosition = mul(gMatPrevWorld, OUT.PrevPosition);
        OUT.PrevPosition = mul(gCamera.MatPrevViewProj, OUT.PrevPosition);

        OUT.Normal = normalize(mul(gMatWorld, float4(OUT.Normal, 0.0f))).xyz;
        OUT.Tangent = normalize(mul(gMatWorld, float4(OUT.Tangent.xyz, 0.0f)));
        OUT.BiTangent = normalize(mul(gMatWorld, float4(OUT.BiTangent.xyz, 0.0f)));

        OUT.UV0 = FlipUV(IN.UV0);
        OUT.UV1 = FlipUV(IN.UV1);

        OUT.PositionWS = mul(gMatWorld, OUT.PositionWS);

#if WRITE_VELOCITY == 1
        OUT.Other.x = (gWriteVelocity == 1) ? 1.0 : 0.0;
#endif
        OUT.Other.y = (gCastLights == 1) ? 1.0 : 0.0;
        OUT.Other.z = (gReceiveShadows == 1) ? 1.0 : 0.0;
    }
    else
    {

#if SKINNED == 1
        if(gInstanceData[instanceid].HasAnimation)
        {
            blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
            prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        }
#endif

        OUT.Position = float4(IN.Position, 1.0f);
        OUT.CurrPosition = float4(IN.Position, 1.0f); // Clip Space
        OUT.PrevPosition = float4(IN.Position, 1.0f); // Clip Space
        OUT.PositionWS = float4(IN.Position, 1.0f); // World Space

        OUT.Normal = IN.Normal;
        OUT.Tangent = IN.Tangent;
        OUT.BiTangent = IN.BiTangent;

#if SKINNED == 1
        if(gHasAnimation)
        {
            OUT.Position = float4(mul(blendMatrix, OUT.Position), 1.0);
            OUT.CurrPosition = float4(mul(blendMatrix, OUT.CurrPosition), 1.0);
            OUT.PrevPosition = float4(mul(prevBlendMatrix, OUT.PrevPosition), 1.0);

            OUT.Normal = mul(blendMatrix, float4(OUT.Normal, 0.0f)).xyz;
            OUT.Tangent = float4(mul(blendMatrix, OUT.Tangent), 0.0f);
            OUT.BiTangent = float4(mul(blendMatrix, OUT.BiTangent), 0.0f);

            OUT.PositionWS = float4(mul(blendMatrix, OUT.PositionWS), 1.0);
        }
#endif

        OUT.Position = mul(gInstanceData[instanceid].MatWorld, OUT.Position);
        OUT.Position = mul(gCamera.MatViewProj, OUT.Position);

        OUT.CurrPosition = mul(gInstanceData[instanceid].MatWorld, OUT.CurrPosition);
        OUT.CurrPosition = mul(gCamera.MatViewProj, OUT.CurrPosition);

        OUT.PrevPosition = mul(gInstanceData[instanceid].MatPrevWorld, OUT.PrevPosition);
        OUT.PrevPosition = mul(gCamera.MatPrevViewProj, OUT.PrevPosition);

        OUT.Normal = normalize(mul(gInstanceData[instanceid].MatWorld, float4(OUT.Normal, 0.0f))).xyz;
        OUT.Tangent = normalize(mul(gInstanceData[instanceid].MatWorld, float4(OUT.Tangent.xyz, 0.0f)));
        OUT.BiTangent = normalize(mul(gInstanceData[instanceid].MatWorld, float4(OUT.BiTangent.xyz, 0.0f)));

        OUT.UV0 = FlipUV(IN.UV0);
        OUT.UV1 = FlipUV(IN.UV1);

        OUT.PositionWS = mul(gInstanceData[instanceid].MatWorld, OUT.PositionWS);

#if WRITE_VELOCITY == 1
        OUT.Other.x = (gInstanceData[instanceid].WriteVelocity == 1) ? 1.0 : 0.0;
#endif
        OUT.Other.y = (gInstanceData[instanceid].CastLights == 1) ? 1.0 : 0.0;
        OUT.Other.z = (gCamera.UseSRGB == 1) ? 1.0 : 0.0;
    }

    float3x3 TBN = float3x3(OUT.Tangent.xyz, OUT.BiTangent.xyz, OUT.Normal);
    OUT.ViewDirWS = normalize(OUT.PositionWS.xyz - gCamera.ViewOrigin);
    OUT.ViewDirTS = mul(TBN, OUT.ViewDirWS);
    OUT.Color = IN.Color;

    // Compute initial parallax displacement direction:
    float2 ParallaxDirection = normalize(  (OUT.ViewDirTS.xy) );

    // The length of this vector determines the furthest amount of displacement:
    float ViewDirLength = length( (OUT.ViewDirTS) );
    float ParallaxLength = sqrt( ViewDirLength * ViewDirLength - (OUT.ViewDirTS.z) * (OUT.ViewDirTS.z) ) / (OUT.ViewDirTS.z);

    // Compute the actual reverse parallax displacement vector:
    OUT.ParallaxOffsetTS = ParallaxDirection * ParallaxLength;

    return OUT;
}
