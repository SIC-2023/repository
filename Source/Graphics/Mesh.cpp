#include "Mesh.h"

#include "d3dx12.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "GraphicsContext.h"

namespace argent::graphics
{
	MeshVertexBuffer::MeshVertexBuffer(ID3D12Device* device, const MeshData& mesh_data)
	{
		position_buffer_ = std::make_unique<VertexBuffer<DirectX::XMFLOAT3>>(device, mesh_data.position_);
		normal_buffer_ = std::make_unique<VertexBuffer<DirectX::XMFLOAT3>>(device, mesh_data.normal_);
		tangent_buffer_ = std::make_unique<VertexBuffer<DirectX::XMFLOAT3>>(device, mesh_data.tangent_);
		binormal_buffer_ = std::make_unique<VertexBuffer<DirectX::XMFLOAT3>>(device, mesh_data.binormal_);
		texcoord_buffer_ = std::make_unique<VertexBuffer<DirectX::XMFLOAT2>>(device, mesh_data.texcoord_);
		bone_buffer_ = std::make_unique<VertexBuffer<BoneVertex>>(device, mesh_data.bone_vertices_);
	}

	void MeshVertexBuffer::SetOnCommandList(ID3D12GraphicsCommandList* graphics_command_list)
	{
		//TODO
		position_buffer_->SetOnCommandList(graphics_command_list, 0u);
		normal_buffer_->SetOnCommandList(graphics_command_list, 1u);
		tangent_buffer_->SetOnCommandList(graphics_command_list, 2u);
		binormal_buffer_->SetOnCommandList(graphics_command_list, 3u);
		texcoord_buffer_->SetOnCommandList(graphics_command_list, 4u);
		//bone_index_buffer_->SetOnCommandList(graphics_command_list, 5u);
		//bone_weight_buffer->SetOnCommandList(graphics_command_list, 6u);
		bone_buffer_->SetOnCommandList(graphics_command_list, 5u);
		//vertex_buffer_->SetOnCommandList(graphics_command_list, 0u);
	}

	Mesh::Mesh(const GraphicsContext& graphics_context, const MeshData& mesh_data) :
		mesh_data_(mesh_data)
	{
		auto* device = graphics_context.device_;
		auto* descriptor_heap = graphics_context.cbv_srv_uav_heap_;
		auto* command_queue = graphics_context.upload_queue_;
		vertex_buffer_ = MeshVertexBuffer(device, mesh_data_);
		index_buffer_ = std::make_unique<IndexBuffer>(device, mesh_data_.indices_);


		//メッシュシェーダーリソース　
		position_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.position_.size(), sizeof(DirectX::XMFLOAT3), mesh_data.position_.data(),
			descriptor_heap->PopDescriptor(), L"Vertex Structured Buffer");

		normal_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.normal_.size(), sizeof(DirectX::XMFLOAT3), mesh_data.normal_.data(),
			descriptor_heap->PopDescriptor(), L"Vertex Structured Buffer");

		tangent_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.tangent_.size(), sizeof(DirectX::XMFLOAT3), mesh_data.tangent_.data(),
			descriptor_heap->PopDescriptor(), L"Vertex Structured Buffer");

		binormal_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.binormal_.size(), sizeof(DirectX::XMFLOAT3), mesh_data.binormal_.data(),
			descriptor_heap->PopDescriptor(), L"Vertex Structured Buffer");

		texcoord_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.texcoord_.size(), sizeof(DirectX::XMFLOAT2), mesh_data.texcoord_.data(),
			descriptor_heap->PopDescriptor(), L"Vertex Structured Buffer");

		bone_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.bone_vertices_.size(), sizeof(BoneVertex), mesh_data.bone_vertices_.data(),
			descriptor_heap->PopDescriptor(), L"Vertex Structured Buffer");

		vertex_index_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.vertex_index_.size(), sizeof(uint32_t), mesh_data.vertex_index_.data(), descriptor_heap->PopDescriptor(),
			L"Vertex Index Structured Buffer");

		meshlet_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.meshlets_.size(), sizeof(Meshlet), mesh_data.meshlets_.data(), descriptor_heap->PopDescriptor(),
			L"Meshlet Structured Buffer");

		primitive_structured_buffer_ = std::make_unique<StructuredBuffer>(device, command_queue,
			mesh_data.primitive_index_.size(), sizeof(PackedPrimitiveData), mesh_data.primitive_index_.data(), descriptor_heap->PopDescriptor()
			, L"Primitive Structured Buffer");

	}

	void Mesh::IASetBuffers(ID3D12GraphicsCommandList* graphics_command_list)
	{
		vertex_buffer_.SetOnCommandList(graphics_command_list);
		index_buffer_->SetOnCommandList(graphics_command_list);
	}
}
