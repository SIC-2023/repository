#include "RootSignature.hlsli"
#include "DemoMeshShader.hlsli"
#include "Constant.h"

uint3 UnpackPrimitiveIndex(uint packed_index)
{
    return uint3(
        packed_index & 0x3FF,
        (packed_index >> 10) & 0x3FF,
        (packed_index >> 20) & 0x3FF);
}


[numthreads(128, 1, 1)]
[outputtopology("triangle")]
[RootSignature(BindlessRootSignature)]
void main
(
    uint gtid /*0~127*/ : SV_GroupThreadID,
    uint gid : SV_GroupID, //CPU側のディスパッチ数 0 ~ Meshletsの数
	in payload Payload payload,
    out vertices MSOutput verts[64],
    out indices uint3 polys[126]
)
{
    StructuredBuffer<Meshlet> meshlets = ResourceDescriptorHeap[render_resource.meshlets_index_];
    StructuredBuffer<uint> primitives = ResourceDescriptorHeap[render_resource.primitives_index_];

    ConstantBuffer<SceneConstant> constant = ResourceDescriptorHeap[render_resource.scene_constant_index_];
    ConstantBuffer<ObjectConstant> object_constant = ResourceDescriptorHeap[render_resource.object_constant_index_];
    ConstantBuffer<MeshConstant> mesh_constant = ResourceDescriptorHeap[render_resource.mesh_constant_index_];
    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[render_resource.position_index_];
    StructuredBuffer<float3> normal_buffer = ResourceDescriptorHeap[render_resource.normal_index_];
    StructuredBuffer<float3> tangent_buffer = ResourceDescriptorHeap[render_resource.tangent_index_];
    StructuredBuffer<float3> binormal_buffer = ResourceDescriptorHeap[render_resource.binormal_index_];
    StructuredBuffer<float2> texcoord_buffer = ResourceDescriptorHeap[render_resource.texcoord_index_];

    StructuredBuffer<uint> indices = ResourceDescriptorHeap[render_resource.vertex_indices_index_];


    uint meshlet_index = payload.meshlet_indices_[gid];
    Meshlet m = meshlets[meshlet_index];
    uint vcount = m.vertex_count_;
    uint pcount = m.primitive_count_;

    SetMeshOutputCounts(vcount, pcount);

    if (gtid < pcount)
    {
        uint packedIndex = primitives[m.primitive_offset_ + gtid /*0~127*/];
        polys[gtid] = UnpackPrimitiveIndex(packedIndex);
    }
    
    if (gtid < m.vertex_count_)
    {
        uint index = indices[m.vertex_offset_ + gtid /*0~127*/];
        float3 position = position_buffer[index];
        float3 normal = normal_buffer[index];
        float3 tangent = tangent_buffer[index];
        float3 binormal = binormal_buffer[index];
        float2 texcoord = texcoord_buffer[index];
        MSOutput output = (MSOutput) 0;

        //row_major float4x4 world = mesh_constant.local_transform_;
        row_major float4x4 world = mul(mesh_constant.local_transform_, object_constant.world_);

        output.position_ = mul(float4(position.xyz, 1.0f), mul(world, constant.camera_data_.view_projection_));

        output.world_position_ = mul(float4(position, 1.0f), world);
        output.normal_ = normalize(mul(float4(normal, 0.0f), world));

        output.tangent_ = normalize(mul(float4(tangent, 0.0f), world));
        output.binormal_ = normalize(mul(float4(binormal, 0.0f), world));

        output.texcoord_ = texcoord;
        verts[gtid] = output;
    }
}