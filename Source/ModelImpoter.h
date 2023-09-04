#pragma once

#include "Graphics/Model.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Graphics/Skeleton.h"

namespace argent::graphics
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position_;
		DirectX::XMFLOAT3 normal_;
		DirectX::XMFLOAT3 tangent_;
		DirectX::XMFLOAT3 binormal_;
		DirectX::XMFLOAT2 texcoord_;
		uint32_t bone_index_[4] { 0, 0, 0, 0 };
		float bone_weights_[4] { 0, 0, 0, 0 };
	};

	Model::Data ImportModel(const char* filename);


	inline DirectX::XMFLOAT3 ToFloat3(aiVector3D vec3)
	{
		return DirectX::XMFLOAT3(vec3.x, vec3.y, vec3.z);
	}
	inline DirectX::XMFLOAT4 ToFloat4(aiQuaternion quaternion)
	{
		return DirectX::XMFLOAT4(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	}
	inline DirectX::XMFLOAT4X4 ToFloat4x4(aiMatrix4x4 mat)
	{
		DirectX::XMFLOAT4X4 ret{};
		ret._11 = mat.a1;
		ret._12 = mat.b1;
		ret._13 = mat.c1;
		ret._14 = mat.d1;

		ret._21 = mat.a2;
		ret._22 = mat.b2;
		ret._23 = mat.c2;
		ret._24 = mat.d2;

		ret._31 = mat.a3;
		ret._32 = mat.b3;
		ret._33 = mat.c3;
		ret._34 = mat.d3;

		ret._41 = mat.a4;
		ret._42 = mat.b4;
		ret._43 = mat.c4;
		ret._44 = mat.d4;
		return ret;
	}

	struct AssimpScene
	{
		struct Node
		{
			std::string name_;
			int32_t index_;
			int32_t parent_index_;
			DirectX::XMFLOAT4X4 transform_;
			DirectX::XMFLOAT4X4 offset_transform_;
			int32_t mesh_counts_;
			std::vector<uint32_t> mesh_indices_;
		};

		std::vector<Node> nodes_;

		int32_t IndexOf(std::string name) const
		{
			int32_t index = 0;
			for(const auto& node : nodes_)
			{
				if(node.name_ == name)
				{
					return index;
				}
				++index;
			}
			return -1;
		}
	};

	static const int kMaxBoneInfluences = 4;

	struct RelayMeshData
	{
		std::string name_;
		std::vector<Vertex> vertices_;
		std::vector<uint32_t> indices_;
		std::vector<Meshlet> meshlets_;
		std::vector<uint32_t> vertex_index_;
		std::vector<PackedPrimitiveData> primitive_index_;
		uint32_t material_index_;
		uint32_t num_polygons_;
		DirectX::XMFLOAT4X4 transform_;
		Skeleton::Data skeleton_data_;
		std::vector<Skeleton::VertexBone> vertex_bones_;
	};
}

