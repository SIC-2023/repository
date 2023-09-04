#include "DemoMeshShader.hlsli"
#include "Constant.h"

//ノーマルマップとサーフェイスノーマル、ワールドタンジェントから法線を求める
float3 GetNormal(float3 surface_normal, float3 surface_binormal, float3 tex_normal, float4 world_tangent)
{
    const float3 N = normalize(surface_normal);
   // const float sigma = worldTangent.w;
    float3 T = normalize(world_tangent.xyz);
    T = normalize(T - N * dot(N, T));
    const float3 B = normalize(surface_binormal /* * sigma*/);
    //const float3 B = normalize(cross(N, T)/* * sigma*/);
    tex_normal = (tex_normal * 2.0f) - 1.0f;
    float3 ret = normalize(tex_normal.r * T + tex_normal.g * B + tex_normal.b * N);

    //float3 ret = normalize(texNormal.b * T + texNormal.r * B + texNormal.g * N);
    return ret;
}

[RootSignature(BindlessRootSignature)]
float4 main(MSOutput input) : SV_TARGET0
{

    float2 texcoord = float2(input.texcoord_.x, 1 - input.texcoord_.y);
    ConstantBuffer<SceneConstant> constant = ResourceDescriptorHeap[render_resource.scene_constant_index_];
    Texture2D base_color_texture = ResourceDescriptorHeap[render_resource.base_color_texture_index_];
    Texture2D normal_texture = ResourceDescriptorHeap[render_resource.normal_texture_index_];

    float4 color = base_color_texture.Sample(linearSampler, texcoord);
    float4 normal_color = normal_texture.Sample(linearSampler, texcoord);

    float3 normal = input.normal_.xyz;
	float3 L = normalize(constant.light_direction_.xyz);
    float3 E = normalize(float3(constant.camera_position_.xyz - input.world_position_.xyz));


#if 0 
    float3 N = GetNormal(input.normal_.xyz, input.binormal_.xyz, normal_color.xyz, input.tangent_);
#else
    float3 N = normalize(normal);
#endif
    float d = max(0, dot(N, -L));

    //float d = max(0, dot(pin.normal_.xyz, -L));
    float3 diffuse = color.rgb * d;

    return float4(diffuse, 1.0f);
}