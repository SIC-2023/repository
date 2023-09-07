#pragma once

#include "../Graphics/GraphicsContext.h"
#include "RenderContext.h"

namespace argent::rendering
{
	/**
	 * \brief �|�X�g�G�t�F�N�g�̎��s�ƊǗ�
	 */
	class PostProcessManager
	{
	public:
		PostProcessManager() = default;
		virtual ~PostProcessManager() = default;

		void OnAwake(const graphics::GraphicsContext& graphics_context);
		void OnShutdown();

		void Execute(const RenderContext& render_context);
	private:

	};
}

