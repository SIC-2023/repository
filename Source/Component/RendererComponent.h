#pragma once

#include "Component.h"

namespace argent::rendering
{
	class RenderContext;
}

namespace argent::component
{
	class RendererComponent :
		public Component
	{
	public:
		RendererComponent(std::string name);
		virtual ~RendererComponent() override = default;

		virtual void OnRender(const rendering::RenderContext& render_context) = 0;
	};
}