#include "ImGui.hlsli"
#include "RootSignature.hlsli"

[RootSignature(BindlessRootSignature)]
VS_OUT main(VS_IN vin)
{
    VS_OUT v_out;
    v_out.position_ = mul(float4(vin.position_.xy, 0.f, 1.f), render_resource.view_projection_matrix_);
    v_out.color_ = vin.color_;
    v_out.texcoord_ = vin.texcoord_;
    return v_out;
}