#include "Light.h"

#include "imgui.h"

#include "../Core/Engine.h"
#include "../Scene/SceneManager.h"
#include "../Rendering/RenderContext.h"

namespace argent::component
{
	Light::Light():
		Component("Direction Light")
	{
		GetEngine()->GetSubsystemLocator().Get<scene::SceneManager>()->GetCurrentScene()->RegisterDirectionLight(this);
	}

	void Light::Awake()
	{
		GetEngine()->GetSubsystemLocator().Get<scene::SceneManager>()->GetCurrentScene()->RegisterDirectionLight(this);
	}

	void Light::OnGui()
	{
		if(ImGui::TreeNode(GetName().c_str()))
		{
			ImGui::DragFloat3("Direction", &direction_.x, 0.001f, -1.0f, 1.0f);
			ImGui::ColorEdit3("Color", &color_.x);
			ImGui::TreePop();
		}
	}
}
