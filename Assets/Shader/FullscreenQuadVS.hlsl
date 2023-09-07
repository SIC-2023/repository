#include "FullscreenQuad.hlsli"
#include "RootSignature.hlsli"

[RootSignature(BindlessRootSignature)]
VSOut main( uint vertex_id : SV_VERTEXID )
{
    VSOut output;

    const float2 position[4] = { float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, -1) };
    const float2 texcoord[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };

    output.position_ = float4(position[vertex_id], 0, 1);
    output.texcoord_ = texcoord[vertex_id];

    return output;
}