#pragma once

#include "../Graphics/GraphicsContext.h"
#include "RenderContext.h"

namespace argent::rendering
{
	class PostProcessManager
	{
	public:
		PostProcessManager(const graphics::GraphicsContext& graphics_context);
		virtual ~PostProcessManager() = default;


		void Execute(const RenderContext& render_context);
	private:

	};
}

