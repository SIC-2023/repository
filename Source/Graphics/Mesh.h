#pragma once
#include <d3d12.h>
#include <DirectXMath.h>

#include <memory>
#include <string>
#include <vector>

#include "StructuredBuffer.h"

//�O���錾
namespace argent::graphics
{
	template<typename T>
	class VertexBuffer;
	class IndexBuffer;

	struct GraphicsContext;
}

namespace argent::graphics
{
	struct BoneVertex
	{
		uint32_t bone_index_[4]{ 0, 0, 0, 0 };
		float bone_weights_[4]{ 0, 0, 0, 0 };
	};

	struct Meshlet
	{
		uint32_t vertex_counts_;
		uint32_t vertex_offsets_;
		uint32_t primitive_counts_;
		uint32_t primitive_offsets_;
		DirectX::XMFLOAT4 bounding_min_;
		DirectX::XMFLOAT4 bounding_max_;
	};

	struct PackedPrimitiveData
	{
		//����Ɉ��k���Ă����炵��
		//�Ȃ̂ł��̍\���̂̃T�C�Y��10 + 10 + 10 + 2 ��32bits(4bytes)
		uint32_t index0 : 10;	
		uint32_t index1 : 10;
		uint32_t index2 : 10;
		uint32_t reserved : 2;
	};

	struct MeshData
	{
		std::string name_;
		std::vector<DirectX::XMFLOAT3> position_{};	//���W
		std::vector<DirectX::XMFLOAT3> normal_{};	//�@��
		std::vector<DirectX::XMFLOAT3> tangent_{};	//�^���W�F���g
		std::vector<DirectX::XMFLOAT3> binormal_{};	//�o�C�m�[�}��
		std::vector<DirectX::XMFLOAT2> texcoord_{};	//uv���W

		//TODO �ʘg�Œu��������������肷��
		std::vector<BoneVertex> bone_vertices_{};
		
		std::vector<uint32_t> indices_{};			//�C���f�b�N�X

		std::vector<Meshlet> meshlets_;				//���b�V�����b�g
		std::vector<uint32_t> vertex_index_;		//���_�o�b�t�@�ɑ΂���C���f�b�N�X
		std::vector<PackedPrimitiveData> primitive_index_;		//�v���~�e�B�u�C���f�b�N�X�@�悭�킩���ĂȂ��� //TODO ���̂��������ł�����R�����g��ύX����

		//TODO ����̃��j�[�NID�Ƃ��ɂ���
		UINT material_index{};

		UINT num_polygon_{};
		DirectX::XMFLOAT4X4 transform_
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
	};

	//���_�o�b�t�@
	struct MeshVertexBuffer
	{
		MeshVertexBuffer() = default;
		MeshVertexBuffer(ID3D12Device* device, const MeshData& mesh_data);
		void SetOnCommandList(ID3D12GraphicsCommandList* graphics_command_list);

		std::unique_ptr<VertexBuffer<DirectX::XMFLOAT3>> position_buffer_{};
		std::unique_ptr<VertexBuffer<DirectX::XMFLOAT3>> normal_buffer_{};
		std::unique_ptr<VertexBuffer<DirectX::XMFLOAT3>> tangent_buffer_{};
		std::unique_ptr<VertexBuffer<DirectX::XMFLOAT3>> binormal_buffer_{};
		std::unique_ptr<VertexBuffer<DirectX::XMFLOAT2>> texcoord_buffer_{};
		std::unique_ptr<VertexBuffer<BoneVertex>> bone_buffer_{};
	};

	//�ʏ�̃��b�V���`��Ɏg��
	//�f�o�b�O�\���p�͕ʂō��
	class Mesh
	{
	public:
		/**
		 * \brief 
		 * \param device DirectX12�̃f�o�C�X�@���_�o�b�t�@�̍쐬�Ɏg��
		 * \param mesh_data ���_�o�b�t�@�̍쐬�Ɏg��
		 */
		Mesh(const GraphicsContext& graphics_context, const MeshData& mesh_data);
		//Mesh(ID3D12Device* device, const MeshData& mesh_data);

		//�R�}���h���X�g�ɂ��̃N���X�������Ă��钸�_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@���o�C���h����
		void IASetBuffers(ID3D12GraphicsCommandList* graphics_command_list);

		size_t GetIndexCount() const { return mesh_data_.indices_.size(); }
		uint32_t GetMaterialIndex() const { return mesh_data_.material_index; }

		uint32_t GetPositionBufferHeapIndex() const { return position_structured_buffer_->GetHeapIndex(); }
		uint32_t GetNormalBufferHeapIndex() const { return normal_structured_buffer_->GetHeapIndex(); }
		uint32_t GetTangentBufferHeapIndex() const { return tangent_structured_buffer_->GetHeapIndex(); }
		uint32_t GetBinormalBufferHeapIndex() const { return binormal_structured_buffer_->GetHeapIndex(); }
		uint32_t GetTexcoordBufferHeapIndex() const { return texcoord_structured_buffer_->GetHeapIndex(); }
		uint32_t GetBoneBufferHeapIndex() const { return bone_structured_buffer_->GetHeapIndex(); }

		uint32_t GetVertexIndexHeapIndex() const { return vertex_index_structured_buffer_->GetHeapIndex(); }
		uint32_t GetMeshletHeapIndex() const { return meshlet_structured_buffer_->GetHeapIndex(); }
		uint32_t GetPrimitiveHeapIndex() const { return primitive_structured_buffer_->GetHeapIndex(); }
		size_t GetNumMeshlets() const { return mesh_data_.meshlets_.size(); }
		uint32_t GetNumPolygon() const { return mesh_data_.num_polygon_; }
		MeshData GetData() const { return mesh_data_; }
	private:
		MeshData mesh_data_{};
		MeshVertexBuffer vertex_buffer_;
		std::unique_ptr<IndexBuffer> index_buffer_{};


		//���b�V���V�F�[�_�[���\�[�X
		std::unique_ptr<StructuredBuffer> position_structured_buffer_;
		std::unique_ptr<StructuredBuffer> normal_structured_buffer_;
		std::unique_ptr<StructuredBuffer> tangent_structured_buffer_;
		std::unique_ptr<StructuredBuffer> binormal_structured_buffer_;
		std::unique_ptr<StructuredBuffer> texcoord_structured_buffer_;
		std::unique_ptr<StructuredBuffer> bone_structured_buffer_;

		std::unique_ptr<StructuredBuffer> vertex_index_structured_buffer_;
		std::unique_ptr<StructuredBuffer> meshlet_structured_buffer_;
		std::unique_ptr<StructuredBuffer> primitive_structured_buffer_;
	};
}

