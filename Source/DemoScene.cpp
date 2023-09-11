#include "DemoScene.h"

#include "Input/InputManager.h"
#include "Core/Engine.h"

#include "Graphics/GraphicsEngine.h"
#include "../Assets/Shader/Constant.h"
#include "Rendering/RenderContext.h"
#include "ModelImpoter.h"
#include "imgui.h"


void DemoScene::OnAwake()
{
	const auto graphics = argent::GetEngine()->GetSubsystemLocator().Get<argent::graphics::GraphicsEngine>();

	//auto model_data = argent::graphics::ImportModel("./Assets/Gear1.gltf");
	//auto model_data = argent::graphics::ImportModel("./Assets/ToyCar.gltf");
	auto model_data = argent::graphics::ImportModel("./Assets/arakBarrak_v025.fbx");
	//auto model_data = argent::graphics::ImportModel("./Assets/ue_Mannequin.fbx");
	//auto model_data = argent::graphics::ImportModel("./Assets/untitled.gltf");
	//auto model_data = argent::graphics::ImportModel("./Assets/untitled2.gltf");

	//TODO ユーザーがモデルを作る必要はないようにする
	std::shared_ptr<argent::graphics::Model> model = std::make_shared<argent::graphics::Model>(
		graphics->GetGraphicsContext(), model_data);
	static_mesh_renderer_ = std::make_unique<argent::rendering::StaticMeshRenderer>(
		graphics->GetGraphicsContext(),	model);
	static_mesh_renderer_1 = std::make_unique<argent::rendering::StaticMeshRenderer>(
		graphics->GetGraphicsContext(),	model);

	sprite_renderer_ = std::make_unique<argent::rendering::SpriteRenderer>(
		graphics->GetGraphicsContext(), L"./Assets/pic001.png");
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

	static_mesh_renderer_->Render(render_context, mesh_transform_.CalcWorldMatrix());
	sprite_renderer_->Render(render_context);
}

void DemoScene::DrawGui()
{
	if (ImGui::TreeNode("Scene"))
	{
		camera_.OnGui();
		mesh_transform_.OnGui();
		static_mesh_renderer_->DrawOnGui();
		sprite_renderer_->DrawOnGui();
		ImGui::DragFloat3("Light Direction", &light_direction_.x, 0.01f, -1.0f, 1.0f);

		ImGui::TreePop();
	}
}
