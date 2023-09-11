#include "FullscreenQuad.hlsli"
#include "RootSignature.hlsli"

struct RenderResource
{
    uint texture_index_;
};
ConstantBuffer<RenderResource> render_resource : register(b0);

[RootSignature(BindlessRootSignature)]
float4 main(VSOut pin) : SV_TARGET
{
    Texture2D texture = ResourceDescriptorHeap[render_resource.texture_index_];
    float4 color = texture.Sample(linearSampler, pin.texcoord_);
    return color;
}