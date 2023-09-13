#include "DemoMeshShader.hlsli"
#include "Constant.h"
#include "CommonCalc.hlsli"

[RootSignature(BindlessRootSignature)]
float4 main(MSOutput input) : SV_TARGET0
{

    float2 texcoord = float2(input.texcoord_.x, 1 - input.texcoord_.y);
    ConstantBuffer<SceneConstant> constant = ResourceDescriptorHeap[render_resource.scene_constant_index_];
    ConstantBuffer<PBRMaterial> material_constant = ResourceDescriptorHeap[render_resource.material_constant_index_];
    Texture2D base_color_texture = ResourceDescriptorHeap[material_constant.base_color_index_];
    Texture2D normal_texture = ResourceDescriptorHeap[material_constant.normal_tex_index_];

    float4 color = base_color_texture.Sample(linearSampler, texcoord);
    float4 normal_color = normal_texture.Sample(linearSampler, texcoord);

    float3 normal = input.normal_.xyz;
	float3 L = normalize(constant.direction_light_.direction_.xyz);
    float3 E = normalize(float3(constant.camera_data_.position_.xyz - input.world_position_.xyz));


#if 1
    float3 N = GetNormal(input.normal_.xyz, input.binormal_.xyz, normal_color.xyz, input.tangent_);
#else
    float3 N = normalize(normal);
#endif
    float d = max(0, dot(N, -L));

    //float d = max(0, dot(pin.normal_.xyz, -L));
    float3 diffuse = color.rgb * d * constant.direction_light_.color_.rgb;

    return float4(diffuse, 1.0f);
}