#include "SpriteRenderer.h"

#include "GraphicsEngine.h"
#include "Shader.h"

//todo
#include "imgui.h"

namespace argent::graphics
{
	struct Constant
	{
		uint32_t texture_index;
	};


	DirectX::XMFLOAT2 CalcScale(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& scale);
	DirectX::XMFLOAT2 CalcRotate(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2 center, float angle);
	DirectX::XMFLOAT3 CalcNdcPosition(const DirectX::XMFLOAT2& world_position, const DirectX::XMFLOAT2& viewport_size);

	SpriteRenderer::SpriteRenderer(const GraphicsContext& graphics_context, const wchar_t* filename)
	{
		texture_ = std::make_unique<graphics::Texture>(graphics_context, filename);

		graphics::GraphicsPipelineDesc graphics_pipeline_desc{};
		graphics_pipeline_desc.vs_filename_ = L"./Assets/Shader/SpriteVS.hlsl";
		graphics_pipeline_desc.ps_filename_ = L"./Assets/Shader/SpritePS.hlsl";
		graphics_pipeline_state_ = std::make_unique<graphics::GraphicsPipelineState>(graphics_context.device_, graphics_pipeline_desc, L"SpritePipeline");

		//頂点バッファ
		std::vector<DirectX::XMFLOAT3> position
		{
			{ -0.5f,  0.5f, 0 },
			{  0.5f,  0.5f, 0 },
			{ -0.5f, -0.5f, 0 },
			{  0.5f, -0.5f, 0 },
		};
		std::vector<DirectX::XMFLOAT2> texcoord
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
		};
		std::vector<DirectX::XMFLOAT4> color
		{
			{ 1, 1, 1, 1},
			{ 1, 1, 1, 1},
			{ 1, 1, 1, 1},
			{ 1, 1, 1, 1}
		};
		position_buffer_ = std::make_unique<graphics::VertexBuffer<DirectX::XMFLOAT3>>(graphics_context.device_, position, false);
		texcoord_buffer_ = std::make_unique<graphics::VertexBuffer<DirectX::XMFLOAT2>>(graphics_context.device_, texcoord, false);
		color_buffer_ = std::make_unique<graphics::VertexBuffer<DirectX::XMFLOAT4>>(graphics_context.device_, color, false);
	}

	void SpriteRenderer::Render(const graphics::RenderContext& render_context)
	{
		const auto graphics_command_list = render_context.GetCommandList();

		//todo
		if(ImGui::TreeNode("Sprite Renderer"))
		{
			ImGui::DragFloat2("Position", &position_.x, 0.1f, 0.0f, 1280.0f);
			ImGui::DragFloat2("Texture Position", &tex_pos_.x, 0.1f, 0.0f, 1280.0f);
			ImGui::DragFloat2("Render Texture Size", &tex_size_.x, 0.1f, 0.0f, 1280.0f);
			ImGui::DragFloat2("Center Offset", &center_offset_.x, 0.1f, 0.0f, 1280.0f);
			ImGui::DragFloat2("Scale", &scale_.x, 0.1f, 0.0f, 1280.0f);
			ImGui::DragFloat("Angle", &angle_, 0.1f, 0.0f, 1280.0f);
			ImGui::ColorPicker4("Color", &color_.x);
			ImGui::TreePop();
		}
		const auto& viewport = render_context.GetViewport();

		const DirectX::XMFLOAT2 viewport_size{ viewport.Width, viewport.Height };
		graphics_pipeline_state_->SetOnCommandList(graphics_command_list);
		graphics_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Constant constant{};
		constant.texture_index = static_cast<uint32_t>(texture_->GetHeapIndex());

		DirectX::XMFLOAT3 position[4]{};
		DirectX::XMFLOAT2 texcoord[4]{};

		const float left = position_.x;
		const float top = position_.y;
		const float right = position_.x + tex_size_.x;
		const float bottom = position_.y + tex_size_.y;

		DirectX::XMFLOAT2 left_top{ position_.x, position_.y };
		DirectX::XMFLOAT2 right_top{ position_.x + tex_size_.x, position_.y };
		DirectX::XMFLOAT2 left_bottom{ position_.x, position_.y + tex_size_.y };
		DirectX::XMFLOAT2 right_bottom{ position_.x + tex_size_.x, position_.y + tex_size_.y };
		
		//to NDC
		left_top = CalcScale(left_top, center_offset_, scale_);
		right_top = CalcScale(right_top, center_offset_, scale_);
		left_bottom = CalcScale(left_bottom, center_offset_, scale_);
		right_bottom = CalcScale(right_bottom, center_offset_, scale_);

		left_top = CalcRotate(left_top, center_offset_, angle_);
		right_top = CalcRotate(right_top, center_offset_, angle_);
		left_bottom = CalcRotate(left_bottom, center_offset_, angle_);
		right_bottom = CalcRotate(right_bottom, center_offset_, angle_);

		position[kLeftTop] = CalcNdcPosition(left_top, viewport_size);
		position[kRightTop] = CalcNdcPosition(right_top, viewport_size);
		position[kLeftBottom] = CalcNdcPosition(left_bottom, viewport_size);
		position[kRightBottom] = CalcNdcPosition(right_bottom, viewport_size);

		texcoord[0].x = tex_pos_.x / texture_->GetWidth();
		texcoord[0].y = tex_pos_.y / texture_->GetHeight();
		texcoord[1].x = (tex_pos_.x + tex_size_.x) / texture_->GetWidth();
		texcoord[1].y = tex_pos_.y / texture_->GetHeight();
		texcoord[2].x = tex_pos_.x / texture_->GetWidth();
		texcoord[2].y = (tex_pos_.y + tex_size_.y) / texture_->GetHeight();
		texcoord[3].x = (tex_pos_.x + tex_size_.x) / texture_->GetWidth();
		texcoord[3].y = (tex_pos_.y + tex_size_.y) / texture_->GetHeight();

		auto w = texture_->GetWidth();
		auto h = texture_->GetHeight();
		DirectX::XMFLOAT4 color[4]{};
		color[0] = color[1] = color[2] = color[3] = color_;

		position_buffer_->UpdateBuffer(position, 4);
		texcoord_buffer_->UpdateBuffer(texcoord, 4);
		color_buffer_->UpdateBuffer(color, 4);

		position_buffer_->SetOnCommandList(graphics_command_list, 0u);
		texcoord_buffer_->SetOnCommandList(graphics_command_list, 1u);
		color_buffer_->SetOnCommandList(graphics_command_list, 2u);

		graphics_command_list->SetGraphicsRoot32BitConstants(0u, 1, &constant, 0u);
		graphics_command_list->DrawInstanced(4u, 1u, 0u, 0u);
	}

	DirectX::XMFLOAT3 CalcNdcPosition(const DirectX::XMFLOAT2& world_position, const DirectX::XMFLOAT2& viewport_size)
	{
		DirectX::XMFLOAT3 ret{};
		ret.x = world_position.x * 2.0f / viewport_size.x - 1.0f;
		ret.y = 1.0f - world_position.y * 2.0f / viewport_size.y;
		ret.z = 0.0f;
		return ret;
	}

	DirectX::XMFLOAT2 CalcScale(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& scale)
	{
		DirectX::XMFLOAT2 ret{};
		const DirectX::XMFLOAT2 dist{ position.x - center.x, position.y - center.y };
		ret.x = center.x + dist.x  * scale.x;
		ret.y = center.y + dist.y * scale.y;
		return ret;
	}

	DirectX::XMFLOAT2 CalcRotate(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2 center, float angle)
	{
		DirectX::XMFLOAT2 ret = position;
		ret.x -= center.x;
		ret.y -= center.y;

		const float cos{ cosf(angle) };
		const float sin{ sinf(angle) };
		const float tx{ ret.x };
		const float ty{ ret.y };

		ret.x = cos * tx - sin * ty;
		ret.y = sin * tx + cos * ty;

		ret.x += center.x;
		ret.y += center.y;

		return ret;
	}
}
