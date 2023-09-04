#pragma once

#include <memory>

#include "SpriteRenderer.h"
#include "Graphics/StaticMeshRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene/BaseScene.h"

class DemoScene:
	public argent::scene::BaseScene
{
public:
	DemoScene():
		BaseScene("Demo")
	{}

	void OnAwake() override;
	void Update() override;
	void Render(const argent::graphics::RenderContext& render_context) override;

private:
	//todo •Ê‚ÌêŠ‚É’u‚­
	argent::Transform mesh_transform_;
	argent::Transform sprite_transform_;
	Camera camera_;
	DirectX::XMFLOAT4 light_direction_{ -1.0f, -1.0f, 1.0, 0.0f };

	std::unique_ptr<argent::graphics::SpriteRenderer> sprite_renderer_{};
	std::unique_ptr<argent::graphics::StaticMeshRenderer> static_mesh_renderer_{};
};

