#pragma once

#include <memory>

#include "RendererComponent.h"

namespace argent::rendering
{
	class SpriteRenderer;
}

namespace argent::component
{
	class ISpriteRenderer final :
		public RendererComponent
	{
	public:
		ISpriteRenderer();
		~ISpriteRenderer() override = default;

		void OnRender(const rendering::RenderContext& render_context) override;

		void OnDrawInspector() override;
	private:
		std::unique_ptr<rendering::SpriteRenderer> sprite_renderer_;
	};
}

