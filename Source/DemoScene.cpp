#include "DemoScene.h"

#include "Input/InputManager.h"
#include "Core/Engine.h"

#include "Graphics/GraphicsEngine.h"
#include "../Assets/Shader/Constant.h"
#include "Rendering/RenderContext.h"
#include "ModelImpoter.h"
#include "imgui.h"

#include "Component/ISpriteRenderer.h"
#include "Component/IStaticMeshRenderer.h"


void DemoScene::Awake()
{

	//sprite_renderer_ = std::make_unique<argent::rendering::SpriteRenderer>(
	//	graphics->GetGraphicsContext(), L"./Assets/pic001.png");

	auto* game_object = AddObject("Object1");
	game_object->AddComponent<argent::component::ISpriteRenderer>();
	game_object->AddComponent<argent::component::IStaticMeshRenderer>();
	auto* child = game_object->AddChild();
	child->AddComponent<argent::component::IStaticMeshRenderer>();
	child->AddChild();
	AddObject("Object2");
	AddObject("Object3");
	AddObject("Object4");
	AddObject("Object5");
	AddObject("Object6");
	AddObject("Object7");
}

void DemoScene::Update()
{
	//TODO カメラコントローラー呼び出し　消すこと
	//カメラコントローラーのチェック
	{
		camera_.Controller();
	}
}

void DemoScene::Render(const argent::rendering::RenderContext& render_context)
{
	SceneConstant scene_data{};
	scene_data.camera_position_ = camera_.GetPosition();
	scene_data.view_projection_ = camera_.GetViewProjection();
	scene_data.light_direction_ = light_direction_;
	render_context.SetSceneData(scene_data);
	render_context.SetFrustumData(scene_data.view_projection_, scene_data.camera_position_);
}

