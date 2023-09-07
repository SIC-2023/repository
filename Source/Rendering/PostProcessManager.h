#pragma once

#include "../Graphics/GraphicsContext.h"
#include "RenderContext.h"

namespace argent::graphics
{
	class PostProcessManager
	{
	public:
		PostProcessManager(const GraphicsContext& graphics_context);
		virtual ~PostProcessManager() = default;


		void Execute(const RenderContext& render_context);
	private:

	};
}

