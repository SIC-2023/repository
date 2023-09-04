#include "RootSignature.hlsli"
#include "DemoMeshShader.hlsli"
#include "Constant.h"

bool IsFrustumCull(float3 aabb_min, float3 aabb_max, float4x4 world, uint frustum_index)
{
    ConstantBuffer<Frustum> frustum = ResourceDescriptorHeap[frustum_index];

    float4 points[8];
    points[0] = float4(aabb_min.x, aabb_min.y, aabb_min.z, 1);
    points[1] = float4(aabb_min.x, aabb_min.y, aabb_max.z, 1);
    points[2] = float4(aabb_min.x, aabb_max.y, aabb_min.z, 1);
    points[3] = float4(aabb_min.x, aabb_max.y, aabb_max.z, 1);
    points[4] = float4(aabb_max.x, aabb_min.y, aabb_min.z, 1);
    points[5] = float4(aabb_max.x, aabb_min.y, aabb_max.z, 1);
    points[6] = float4(aabb_max.x, aabb_max.y, aabb_min.z, 1);
    points[7] = float4(aabb_max.x, aabb_max.y, aabb_max.z, 1);

    for (int point_id = 0; point_id < 8; point_id++)
        points[point_id] = mul(points[point_id], world);

    for (int plane_id = 0; plane_id < 6; plane_id++)
    {
        float3 plane_normal = frustum.frustum_panels_[plane_id].xyz;
        float plane_constant = frustum.frustum_panels_[plane_id].w;

        bool inside = false;
        for (int point_id = 0; point_id < 8; point_id++)
        {
            if (dot(plane_normal, points[point_id].xyz) + plane_constant >= 0)
            {
                inside = true;
                break;
            }
        }

        if (!inside)
        {
            return true;
        }
    }
    return false;
}

groupshared Payload s_payload;


[numthreads(32, 1, 1)]
[RootSignature(BindlessRootSignature)]
void main(uint gtid : SV_GroupThreadID,
	uint dtid : SV_DispatchThreadID,
	uint gid : SV_GroupID)
{
    StructuredBuffer<Meshlet> meshlets = ResourceDescriptorHeap[render_resource.meshlets_index_];
    ConstantBuffer<ObjectConstant> object_constant = ResourceDescriptorHeap[render_resource.object_constant_index_];
    ConstantBuffer<MeshConstant> mesh_constant = ResourceDescriptorHeap[render_resource.mesh_constant_index_];


	uint num, stride;
    meshlets.GetDimensions(num, stride);

    bool visible = false;
    if (dtid < num)
    {
        Meshlet m = meshlets[dtid];

        if (!IsFrustumCull(m.bounding_min_.xyz, m.bounding_max_.xyz, mul(mesh_constant.local_transform_, object_constant.world_), render_resource.frustum_))
        {
            visible = true;
        }
    }

    if (visible)
    {
        //このスレッドよりも小さいスレッドのみ対象
        //引数に入れた変数がtrueの数が帰ってくる
        uint index = WavePrefixCountBits(visible);
        s_payload.meshlet_indices_[index] = dtid;
    }

    //すべてのスレッドで引数に入れた変数がtrueの数が帰ってくる
    uint visible_count = WaveActiveCountBits(visible);
    DispatchMesh(visible_count, 1, 1, s_payload);
}