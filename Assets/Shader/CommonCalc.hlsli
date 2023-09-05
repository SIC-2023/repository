
//ノーマルマップとサーフェイスノーマル、ワールドタンジェントから法線を求める
float3 GetNormal(float3 surface_normal, float3 surface_binormal, float3 tex_normal, float4 world_tangent)
{
    const float3 N = normalize(surface_normal);
    float3 T = normalize(world_tangent.xyz);
    T = normalize(T - N * dot(N, T));
    const float3 B = normalize(surface_binormal);
    tex_normal = (tex_normal * 2.0f) - 1.0f;
    float3 ret = normalize(tex_normal.r * T + tex_normal.g * B + tex_normal.b * N);
    return ret;
}

