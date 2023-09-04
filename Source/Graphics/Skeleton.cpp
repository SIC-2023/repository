#include "Skeleton.h"

namespace argent::graphics
{
	Skeleton::Skeleton(ID3D12Device* device, const Data& data)
	{
		data_ = data;
		//vertex_buffer_ = std::make_unique<VertexBuffer<VertexBone>>(device, data.vertex_bones_);
	}

	void Skeleton::SetOnCommandList(ID3D12GraphicsCommandList6* graphics_command_list, UINT start_slot) const
	{
		vertex_buffer_->SetOnCommandList(graphics_command_list, start_slot);
	}
}
