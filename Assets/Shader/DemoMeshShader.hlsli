#include "RootSignature.hlsli"

struct MSOutput
{
    float4 position_ : SV_POSITION;
    float4 world_position_ : POSITION;
    float4 normal_ : NORMAL;
    float4 tangent_ : TANGENT;
    float4 binormal_ : BITANGENT;
    float2 texcoord_ : TEXCOORD;
};

struct Meshlet
{
    uint vertex_count_; // �o�͒��_��.
    uint vertex_offset_; // ���_�ԍ��I�t�Z�b�g.
    uint primitive_count_; // �o�̓v���~�e�B�u��.
    uint primitive_offset_; // �v���~�e�B�u�ԍ��I�t�Z�b�g.
    float4 bounding_min_;
    float4 bounding_max_;
};

struct Payload
{
    uint meshlet_indices_[32];
};

struct RenderResource
{
    uint scene_constant_index_;
    uint object_constant_index_;
    uint mesh_constant_index_;
    uint base_color_texture_index_;
    uint normal_texture_index_;
    uint vertices_index_;
    uint vertex_indices_index_;
    uint meshlets_index_;
    uint primitives_index_;
    uint frustum_;
    uint instance_index_;
    uint position_index_;
    uint normal_index_;
    uint tangent_index_;
    uint binormal_index_;
    uint texcoord_index_;
    uint bone_index_;
};


ConstantBuffer<RenderResource> render_resource : register(b0);