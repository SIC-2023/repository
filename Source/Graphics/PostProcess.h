#pragma once

#include "GraphicsContext.h"
#include "RenderContext.h"

namespace argent::graphics
{
	class PostProcess
	{
	public:
		PostProcess(const GraphicsContext& graphics_context);

		virtual ~PostProcess() = default;

		virtual void OnBegin(const RenderContext& render_context);
	private:
	};
}

