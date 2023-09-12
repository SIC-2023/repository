#include "IStaticMeshRenderer.h"

#include "../Core/Engine.h"
#include "../Graphics/GraphicsEngine.h"
#include "../ModelImpoter.h"
#include "../GameObject/GameObject.h"

#include "../Rendering/StaticMeshRenderer.h"

namespace argent::component
{
	std::shared_ptr<argent::graphics::Model> model = nullptr;
	IStaticMeshRenderer::IStaticMeshRenderer():
		RendererComponent("Static Mesh Renderer")
	{
		const auto graphics = argent::GetEngine()->GetSubsystemLocator().Get<argent::graphics::GraphicsEngine>();

		if(!model)
		{
			auto model_data = argent::graphics::ImportModel("./Assets/Gear1.gltf");
			//auto model_data = argent::graphics::ImportModel("./Assets/ToyCar.gltf");
			//auto model_data = argent::graphics::ImportModel("./Assets/arakBarrak_v025.fbx");
			//auto model_data = argent::graphics::ImportModel("./Assets/ue_Mannequin.fbx");
			//auto model_data = argent::graphics::ImportModel("./Assets/untitled.gltf");
			//auto model_data = argent::graphics::ImportModel("./Assets/untitled2.gltf");

			//TODO ユーザーがモデルを作る必要はないようにする
			model = std::make_shared<argent::graphics::Model>(
				graphics->GetGraphicsContext(), model_data);
		}
		static_mesh_renderer_ = std::make_unique<argent::rendering::StaticMeshRenderer>(
			graphics->GetGraphicsContext(),	model);
	}

	void IStaticMeshRenderer::Update()
	{
		if(GetAsyncKeyState(0x0058) & 0x8000)
		{
			DirectX::XMFLOAT3 pos = GetOwner()->GetTransform()->GetPosition();
			pos.z += 0.1f;
			GetOwner()->GetTransform()->SetPosition(pos);
		}
	}

	void IStaticMeshRenderer::OnRender(const rendering::RenderContext& render_context)
	{
		static_mesh_renderer_->Render(render_context, GetOwner()->GetTransform()->CalcWorldMatrix());
	}

	void IStaticMeshRenderer::OnGui()
	{
		static_mesh_renderer_->DrawOnGui();
	}
}
