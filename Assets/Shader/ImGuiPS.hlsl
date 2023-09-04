#include "ImGui.hlsli"
#include "RootSignature.hlsli"

[RootSignature(BindlessRootSignature)]
float4 main(VS_OUT pin) : SV_TARGET
{
    Texture2D texture = ResourceDescriptorHeap[render_resource.texture_index_];
    float4 color = pin.color_ * texture.Sample(linearSampler, pin.texcoord_);
    return color;
}