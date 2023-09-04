#include "RootSignature.hlsli"
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct Constant
{
    uint texture_index;
};
ConstantBuffer<Constant> render_resource : register(b0);
[RootSignature(BindlessRootSignature)]
float4 main(VS_OUT pin) : SV_TARGET
{
    Texture2D texture = ResourceDescriptorHeap[render_resource.texture_index];

    float4 color =  texture.Sample(linearSampler, pin.texcoord) * pin.color;
    return color;
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}