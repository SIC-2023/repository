#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <vector>

namespace argent::graphics
{
	//�C���f�b�N�X�o�b�t�@
	//uint32_t�^�Ńf�[�^���i�[���邱��
	class IndexBuffer
	{
	public:
		IndexBuffer(ID3D12Device* device, const std::vector<uint32_t>& indices);

		void SetOnCommandList(ID3D12GraphicsCommandList* command_list) const
		{
			command_list->IASetIndexBuffer(&index_buffer_view_);
		}

	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> index_buffer_{};
		D3D12_INDEX_BUFFER_VIEW index_buffer_view_{};
	};
}

