#pragma once
#include <d3d12.h>
#include <DirectXMath.h>

#include <memory>
#include <vector>

#include "Mesh.h"
#include "Material.h"
#include "Skeleton.h"
#include "AnimationClip.h"

namespace argent::graphics
{
	template<typename T>
	class VertexBuffer;
	class IndexBuffer;
	template<typename T>
	class ConstantBuffer;

	struct GraphicsContext;

}


namespace argent::graphics
{
	class Model
	{
	public:
		struct Node
		{
			std::string name_;
			int32_t unique_id_;
			int32_t parent_unique_id_;
			DirectX::XMFLOAT4X4 transform_;
			DirectX::XMFLOAT4X4 offset_transform_;
			std::vector<Node> children_;
			void OnGui();
		};

		struct Data
		{
			std::vector<Node> nodes_;
			std::vector<MeshData> mesh_data_{};
			std::vector<PbrMaterial::Data> material_data_{};
			std::vector<AnimationClip> animation_clips_;
			uint32_t num_polygons_{};
		};
	public:

		//Model(ID3D12Device* device, const Data& model_data);
		Model(const GraphicsContext& graphics_context, const Data& model_data);
		void OnGui();


		size_t GetNumMeshes() const { return meshes_.size(); }
		std::shared_ptr<Mesh> GetMesh(size_t index) { return meshes_.at(index); }
		size_t GetNumMaterials() const { return materials_.size(); }
		std::shared_ptr<PbrMaterial> GetMaterial(size_t index) { return materials_.at(index); }

		uint32_t GetNumPolygons() const { return data_.num_polygons_; }
	private:
		Data data_;

		std::vector<std::shared_ptr<Mesh>> meshes_{};
		std::vector<std::shared_ptr<PbrMaterial>> materials_{};
	};
}

