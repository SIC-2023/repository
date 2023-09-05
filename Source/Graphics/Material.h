#pragma once
#include <d3d12.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "Texture.h"


//前方宣言
namespace argent::graphics::dx12
{
	class Descriptor;
}
namespace argent::graphics
{
	class Texture;
	template<typename T>
	class ConstantBuffer;

	struct GraphicsContext;
}

namespace argent::graphics
{
	//TODO 仮置き　pbr以外　すべてのマテリアルに対応できるやつを作りたい
	class PbrMaterial
	{
	public:
		enum class TextureUsage { BaseColor, Normal, Metallic, Roughness };

		struct Constant
		{
			uint32_t base_color_index_;
			uint32_t normal_tex_index_;
			uint32_t metallic_tex_index_;
			uint32_t roughness_tex_index_;
			float metallic_factor_;
			float roughness_factor_;
		};

		struct Data
		{
			std::string name_;
			std::unordered_map<TextureUsage, std::string> texture_data_;
		};

	public:
		PbrMaterial(const GraphicsContext& graphics_context, const Data& data);

		void UpdateConstantBuffer();
		uint64_t GetHeapIndex() const;
		void OnGui();

	private:
		Data data_;
		std::unordered_map<TextureUsage, std::unique_ptr<Texture>> textures_;
		float metallic_factor_{};
		float roughness_factor_{};
		std::unique_ptr<ConstantBuffer<Constant>> constant_buffer_;
	};
}

