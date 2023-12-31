#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "../Graphics/GraphicsPipelineState.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/Texture.h"

namespace argent::graphics
{
	struct GraphicsContext;
}

namespace argent::rendering
{
	class RenderContext;
}

namespace argent::rendering
{
	class SpriteRenderer
	{
	public:
		SpriteRenderer(const graphics::GraphicsContext& graphics_context, const wchar_t* filename);

	private:
		static constexpr int kLeftTop = 0;
		static constexpr int kRightTop = 1;
		static constexpr int kLeftBottom = 2;
		static constexpr int kRightBottom = 3;

	public:

		void Render(const rendering::RenderContext& render_context);
		void DrawOnGui();
	private:

		std::unique_ptr<graphics::GraphicsPipelineState> graphics_pipeline_state_{};

		Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer_{};
		std::unique_ptr<graphics::VertexBuffer<DirectX::XMFLOAT3>> position_buffer_{};
		std::unique_ptr<graphics::VertexBuffer<DirectX::XMFLOAT2>> texcoord_buffer_{};
		std::unique_ptr<graphics::VertexBuffer<DirectX::XMFLOAT4>> color_buffer_{};

		std::unique_ptr<graphics::Texture> texture_{};

		//todo  外に出したい
		DirectX::XMFLOAT2 position_{};
		DirectX::XMFLOAT2 tex_pos_{};
		DirectX::XMFLOAT2 tex_size_{ 120, 120 };
		DirectX::XMFLOAT2 center_offset_{ 0, 0 };
		DirectX::XMFLOAT2 scale_{ 1, 1 };
		DirectX::XMFLOAT4 color_{ 1, 1, 1, 1 };
		float angle_{};	//Radian
	};
}

