#include "StaticMesh.hlsli"
#include "RootSignature.hlsli"
#include "Constant.h"

[RootSignature(BindlessRootSignature)]
VertexShaderOutput main(VertexShaderInput vin, uint instance_index : SV_INSTANCEID)
{
    ConstantBuffer<SceneConstant> constant = ResourceDescriptorHeap[render_resource.scene_constant_index_];
    ConstantBuffer<ObjectConstant> object_constant = ResourceDescriptorHeap[render_resource.object_constant_index_];
    ConstantBuffer<MeshConstant> mesh_constant = ResourceDescriptorHeap[render_resource.mesh_constant_index_];
    VertexShaderOutput vout;


    row_major float4x4 world = mul(mesh_constant.local_transform_, object_constant.world_);

    vout.position = mul(float4(vin.position, 1.0f), mul(world, constant.camera_data_.view_projection_));
    vout.world_position = mul(float4(vin.position, 1.0f), world);
    vout.normal = normalize(mul(float4(vin.normal, 0.0f), world));

    vout.tangent = normalize(mul(float4(vin.tangent, 0.0f), world));
    vout.binormal = normalize(mul(float4(vin.bitangent, 0.0f), world));

    vout.texcoord = vin.texcoord;
    return vout;
}