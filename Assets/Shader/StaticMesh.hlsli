struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texcoord : TEXCOORD;
    uint4 bone_index : BONEINDEX;
    float4 bone_weights : BONEWEIGHTS;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BITANGENT;
    float2 texcoord : TEXCOORD;
};

struct RenderResource
{
    uint scene_constant_index_;
    uint object_constant_index_;
    uint mesh_constant_index_;
    //uint base_color_texture_index_;
    //uint normal_texture_index_;
    uint material_constant_index_;
    uint vertices_index_;
    uint vertex_indices_index_;
    uint meshlets_index_;
    uint primitives_index_;
    uint frustum_;
    uint instance_index_;
};



ConstantBuffer<RenderResource> render_resource : register(b0);