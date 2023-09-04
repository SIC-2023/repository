#pragma once

#include <DirectXMath.h>

#include <memory>
#include <string>
#include <vector>

#include "VertexBuffer.h"

namespace argent::graphics
{
	class Skeleton
	{
	public:
		struct VertexBone
		{
			uint32_t bone_index_[4];
			float weights_[4];
		};

		struct Data
		{
			struct Bone
			{
				std::string	name_{};
				uint32_t unique_id_;//�{�[���z��̃C���f�b�N�X
				uint32_t node_index_;	//AssimpScene�̃m�[�h�ɑ΂���A�N�Z�X
				uint32_t parent_node_index_;//��ɓ�����
				std::string parent_name_{};
				DirectX::XMFLOAT4X4 offset_transform_
				{
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1,
				};
			};
			std::vector<Bone> bones_{};

			int32_t IndexOf(std::string name) const
			{
				int32_t index = 0;
				for(const auto& bone : bones_)
				{
					if(bone.name_ == name)
					{
						return index;
					}
					++index;
				}
				return -1;
			}

			//�{�[���f�[�^��ǉ�����
			//���łɊi�[���Ă���f�[�^�Ɩ��O���r���ē������̂��������ꍇ�͉������Ȃ�
			void Add(Bone& bone)
			{
				for(const auto& b : bones_)
				{
					if(b.name_ == bone.name_)
					{
						return;
					}
				}
				bone.unique_id_ = static_cast<uint32_t>(bones_.size());
				bones_.emplace_back(bone);
			}
		};

	public:
		Skeleton(ID3D12Device* device, const Data& data);

		void SetOnCommandList(ID3D12GraphicsCommandList6* graphics_command_list, UINT start_slot) const;
	private:
		std::vector<VertexBone> vertex_bones_{};
		Data data_{};
		std::unique_ptr<VertexBuffer<VertexBone>> vertex_buffer_{};
	};
}

