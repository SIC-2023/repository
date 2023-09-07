#include "StaticMesh.hlsli"
#include "RootSignature.hlsli"
#include "Constant.h"
#include "CommonCalc.hlsli"

[RootSignature(BindlessRootSignature)]
float4 main(VertexShaderOutput pin) : SV_TARGET
{
    ConstantBuffer<SceneConstant> constant = ResourceDescriptorHeap[render_resource.scene_constant_index_];
    ConstantBuffer<PBRMaterial> material_constant = ResourceDescriptorHeap[render_resource.material_constant_index_];
    Texture2D base_color_texture = ResourceDescriptorHeap[material_constant.base_color_index_];
    Texture2D normal_texture = ResourceDescriptorHeap[material_constant.normal_tex_index_];

#if 1
    float2 texcoord = float2(pin.texcoord.x, 1.0f - pin.texcoord.y);

#else
    float2 texcoord = float2(pin.texcoord.x, pin.texcoord.y);
#endif

    float4 color = base_color_texture.Sample(linearSampler, texcoord);
    float4 normal_color = normal_texture.Sample(linearSampler, texcoord);

    float3 normal = pin.normal.xyz;
    float3 L = normalize(constant.light_direction_.xyz);
    float3 E = normalize(float3(constant.camera_position_.xyz - pin.world_position.xyz));

#if 1 
    float3 N = GetNormal(pin.normal.xyz, pin.binormal.xyz, normal_color.xyz, pin.tangent);
#else
    float3 N = pin.normal.xyz;
#endif
    float d = max(0, dot(N, -L));
    //float d = max(0, dot(pin.normal.xyz, -L));
    float3 diffuse = color.rgb * d;
    float3 ambient = color.rgb * 0.1f;

    return float4(diffuse + ambient, 1.0f);
}