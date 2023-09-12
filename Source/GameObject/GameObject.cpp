#include "GameObject.h"

#include "imgui.h"

namespace argent
{
	GameObject::GameObject(std::string name):
		name_(name)
	{
		transform_ = AddComponent<component::Transform>();
	}

	void GameObject::Start()
	{
		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->Start();
		}

		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			child_objects_.at(i)->Start();
		}
	}

	void GameObject::Awake()
	{
		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->Awake();
		}

		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			child_objects_.at(i)->Awake();
		}
	}

	void GameObject::OnShutDown()
	{
		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->Shutdown();
		}

		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			child_objects_.at(i)->OnShutDown();
		}
	}

	void GameObject::OnUpdate()
	{
		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->Update();
		}

		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			child_objects_.at(i)->OnUpdate();
		}
	}

	void GameObject::OnDrawInspector()
	{
		ImGui::Text(name_.c_str());
		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->OnGui();
		}
	}

	GameObject* GameObject::AddChild()
	{
		child_objects_.emplace_back(std::make_unique<GameObject>("Child"));
		return  (*child_objects_.rbegin()).get();
	}
}
