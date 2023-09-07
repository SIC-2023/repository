#pragma once

#include "../Graphics/GraphicsContext.h"
#include "RenderContext.h"

namespace argent::rendering
{
	class PostProcess
	{
	public:
		PostProcess(const graphics::GraphicsContext& graphics_context);

		virtual ~PostProcess() = default;

		virtual void OnBegin(const RenderContext& render_context);
	private:
	};
}

