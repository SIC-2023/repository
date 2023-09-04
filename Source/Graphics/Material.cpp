#include "Material.h"

#include "ConstantBuffer.h"
#include "Texture.h"
#include "Dx12/DescriptorHeap.h"

namespace argent::graphics
{
	//PbrMaterial::PbrMaterial(ID3D12Device* device, ID3D12CommandQueue* command_queue,
	//	dx12::DescriptorHeap* srv_descriptor_heap, const Data& data)
	//{
	//	//マップはコピー禁止なのでfor文で全部移していく
	//	for(const auto& d : data.texture_data_)
	//	{
	//		data_.texture_data_[d.first] = d.second;
	//	}
	//	data_.name_ = data.name_;

	//	for(const auto& d : data.texture_data_)
	//	{
	//		textures_[d.first] = std::make_unique<Texture>(device, command_queue, d.second.c_str(), srv_descriptor_heap->PopDescriptor());
	//		//textures_[d.first] = std::make_unique<Texture>(device, command_queue, d.second.c_str(), srv_descriptor_heap->PopDescriptor());
	//	}
	//}

	PbrMaterial::PbrMaterial(const GraphicsContext& graphics_context, const Data& data)
	{
		//マップはコピー禁止なのでfor文で全部移していく
		for (const auto& d : data.texture_data_)
		{
			data_.texture_data_[d.first] = d.second;
		}
		data_.name_ = data.name_;

		for (const auto& d : data.texture_data_)
		{
			textures_[d.first] = std::make_unique<Texture>(graphics_context, d.second.c_str());
			//textures_[d.first] = std::make_unique<Texture>(device, command_queue, d.second.c_str(), srv_descriptor_heap->PopDescriptor());
		}

	}

	void PbrMaterial::UpdateConstantBuffer()
	{
		//TODO デフォルトのテクスチャ番号を用意しておくこと
		Constant constant{};
		constant.base_color_index_ = textures_.contains(TextureUsage::BaseColor) ? static_cast<uint32_t>(textures_[TextureUsage::BaseColor]->GetHeapIndex()) : 0u; 
		constant.normal_tex_index_ = textures_.contains(TextureUsage::Normal) ? static_cast<uint32_t>(textures_[TextureUsage::Normal]->GetHeapIndex()) : 0u; 
		constant.metallic_tex_index_ = textures_.contains(TextureUsage::Metallic) ? static_cast<uint32_t>(textures_[TextureUsage::Metallic]->GetHeapIndex()) : 0u; 
		constant.roughness_tex_index_ = textures_.contains(TextureUsage::Roughness) ? static_cast<uint32_t>(textures_[TextureUsage::Roughness]->GetHeapIndex()) : 0u;
		constant.metallic_factor_ = metallic_factor_;
		constant.roughness_factor_ = roughness_factor_;
		constant_buffer_->UpdateConstantBuffer(constant);
	}

	uint64_t PbrMaterial::GetHeapIndex() const
	{
		return constant_buffer_->GetHeapIndex();
	}
}
