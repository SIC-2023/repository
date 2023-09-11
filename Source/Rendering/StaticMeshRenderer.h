#pragma once
#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/GraphicsPipelineState.h"
#include "../Graphics/Model.h"


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
	/**
	 * \brief �{�[���Ȃ�3D���f���`��N���X
	 */
	class StaticMeshRenderer
	{
	public:
		struct ObjectConstant
		{
			DirectX::XMFLOAT4X4 world_;
		};

		struct MeshConstant
		{
			DirectX::XMFLOAT4X4 local_transform_;
		};

	public:
		StaticMeshRenderer(const graphics::GraphicsContext& graphics_context, const std::shared_ptr<graphics::Model>& model,
			const wchar_t* ps_filename = L"./Assets/Shader/StaticMeshPS.hlsl");

		void Render(const RenderContext& render_context, const DirectX::XMFLOAT4X4& world);

		void DrawOnGui();
	private:

		//�X�^�e�B�b�N���b�V�������o
		std::unique_ptr<graphics::GraphicsPipelineState> graphics_pipeline_state_{};
		std::unique_ptr<graphics::GraphicsPipelineState> mesh_shader_pipeline_state_{};
		std::shared_ptr<graphics::Model> model_{};

		std::unique_ptr<graphics::ConstantBuffer<ObjectConstant>> object_constant_buffer_[3]{};
		std::vector<std::unique_ptr<graphics::ConstantBuffer<MeshConstant>>> mesh_constant_buffers_[3]{};

		//TODO ���b�V���V�F�[�_�[���g�����ǂ����@�ꍇ�������ǂ�����H�H
		bool use_mesh_shader_ = false;
	};
}

