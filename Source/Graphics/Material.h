#pragma once
#include <d3d12.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "Texture.h"
#include "ConstantBuffer.h"
#include "GraphicsContext.h"


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

	class IMaterial
	{
	public:
		IMaterial() = default;
		virtual ~IMaterial() = default;

		virtual void UpdateConstantBuffer() = 0;
		virtual uint64_t GetHeapIndex() const = 0;
		virtual void OnGui() = 0;
		virtual std::string GetName() =0;
	};

	template <class T>
	class Material : public IMaterial
	{
	public:
		Material(std::string name):
		name_(name)
		{}

		virtual ~Material() override = default;

		uint64_t GetHeapIndex() const override final { return constant_buffer_->GetHeapIndex(); }
		void OnGui() override {}
		std::string GetName() override { return name_; }

	protected:
		std::string name_;
		std::unique_ptr<ConstantBuffer<T>> constant_buffer_;
	};


	//TODO 仮置き　pbr以外　すべてのマテリアルに対応できるやつを作りたい
	struct PbrMaterialData
	{
		uint32_t base_color_index_;
		uint32_t normal_tex_index_;
		uint32_t metallic_tex_index_;
		uint32_t roughness_tex_index_;
		float metallic_factor_;
		float roughness_factor_;
	};

	class PbrMaterial : public  Material<PbrMaterialData>
	{
	public:
		enum class TextureUsage { BaseColor, Normal, Metallic, Roughness };

		struct Data
		{
			std::string name_;
			std::unordered_map<TextureUsage, std::string> texture_data_;
		};

	public:
		PbrMaterial(const GraphicsContext& graphics_context, const Data& data);

		void UpdateConstantBuffer() override;
		void OnGui() override;

	private:
		Data data_;
		std::unordered_map<TextureUsage, std::unique_ptr<Texture>> textures_;
		float metallic_factor_{};
		float roughness_factor_{};
	};
}

