#include "ISpriteRenderer.h"

#include "../Core/Engine.h"
#include "../Graphics/GraphicsEngine.h"

#include "../Rendering/SpriteRenderer.h"
namespace argent::component
{
	ISpriteRenderer::ISpriteRenderer():
		RendererComponent("Sprite Renderer")
	{
		const auto graphics = argent::GetEngine()->GetSubsystemLocator().Get<argent::graphics::GraphicsEngine>();

		sprite_renderer_ = std::make_unique<argent::rendering::SpriteRenderer>(
		graphics->GetGraphicsContext(), L"./Assets/pic001.png");
	}

	void ISpriteRenderer::OnRender(const rendering::RenderContext& render_context)
	{
		sprite_renderer_->Render(render_context);
	}

	void ISpriteRenderer::OnDrawInspector()
	{
		sprite_renderer_->DrawOnGui();
	}
}
