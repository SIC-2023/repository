#include "BaseScene.h"

#include "imgui.h"

#include "../Editor/InspectorWindow.h"

namespace argent::scene
{
	void BaseScene::Awake()
	{
		for(size_t i = 0; i < game_objects_.size(); ++i)
		{
			if(game_objects_.at(i)->GetIsActive()) game_objects_.at(i)->Awake();
		}

		for(size_t i = 0; i < game_objects_.size(); ++i)
		{
			if(game_objects_.at(i)->GetIsActive()) game_objects_.at(i)->OnEnable();
		}

		for(size_t i = 0; i < game_objects_.size(); ++i)
		{
			if(game_objects_.at(i)->GetIsActive()) game_objects_.at(i)->Start();
		}
	}

	void BaseScene::Shutdown()
	{
		for(size_t i = 0; i < game_objects_.size(); ++i)
		{
			if(game_objects_.at(i)->GetIsActive()) game_objects_.at(i)->OnDisable();
		}

		for(size_t i = 0; i < game_objects_.size(); ++i)
		{
			if(game_objects_.at(i)->GetIsActive()) game_objects_.at(i)->OnDestroy();
		}
	}

	void BaseScene::Update()
	{
		for(size_t i = 0; i < game_objects_.size(); ++i)
		{
			if(game_objects_.at(i)->GetIsActive()) game_objects_.at(i)->Update();
		}
	}

	bool CheckMouseFocused(ImVec2 left_top, ImVec2 size, ImVec2 local_mouse_pos)
	{
		if(local_mouse_pos.x < left_top.x) return false;
		if(local_mouse_pos.x > left_top.x + size.x) return false;
		if(local_mouse_pos.y < left_top.y) return false;
		if(local_mouse_pos.y > left_top.y + size.y) return false;
		return true;
	}

	void DrawInspector(GameObject* game_object)
	{
		if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			const ImVec2 mouse_pos = ImGui::GetMousePos();
			const ImVec2 window_pos = ImGui::GetWindowPos();
			const ImVec2 local_mouse_pos = { mouse_pos.x - window_pos.x, mouse_pos.y - window_pos.y };

			auto widget_left_top = ImGui::GetCursorPos();
			auto text_size = ImGui::CalcTextSize(game_object->GetName().c_str());
			if(ImGui::TreeNodeEx(game_object->GetName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
			{
				for(size_t i = 0; i < game_object->GetChildCounts(); ++i)
				{
					DrawInspector(game_object->GetChild(i));
				}
				ImGui::TreePop();
			}

			auto item_rect = ImGui::GetItemRectSize();
			ImVec2 size = { item_rect.x, item_rect.y };
			ImVec2 left_top = { widget_left_top.x + size.y, widget_left_top.y };
			if(CheckMouseFocused(left_top, size, local_mouse_pos))
			{
				editor::InspectorWindow::focused_game_object_ = game_object;
			}
		}
		else
		{
			if(ImGui::TreeNodeEx(game_object->GetName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
			{
				for(size_t i = 0; i < game_object->GetChildCounts(); ++i)
				{
					DrawInspector(game_object->GetChild(i));
				}
				ImGui::TreePop();
			}
		}
	}

	void BaseScene::OnGui()
	{
		for(size_t i = 0; i < game_objects_.size(); ++i)
		{
			DrawInspector(game_objects_.at(i).get());
		}
	}

	GameObject* BaseScene::AddObject(std::string name)
	{
		game_objects_.emplace_back(std::make_unique<GameObject>(name));
		return (*game_objects_.rbegin()).get();
	}
}
