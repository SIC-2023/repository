#pragma once

#include <memory>

#include "Rendering/SpriteRenderer.h"
#include "Rendering/StaticMeshRenderer.h"
#include "Component/Camera.h"
#include "Scene/BaseScene.h"

class DemoScene:
	public argent::scene::BaseScene
{
public:
	DemoScene():
		BaseScene("Demo")
	{}

	void Awake() override;
	void Update() override;
	void Render(const argent::rendering::RenderContext& render_context) override;

private:
	//TODO ComponentŠ®¬‚µ‚½‚çˆÚ“®‚³‚¹‚é‚±‚Æ
	argent::component::Camera camera_;
	DirectX::XMFLOAT4 light_direction_{ -1.0f, -1.0f, 1.0, 0.0f };
};

