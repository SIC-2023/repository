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

	void GameObject::OnAwake()
	{
		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->OnAwake();
		}

		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			child_objects_.at(i)->OnAwake();
		}
	}

	void GameObject::OnShutDown()
	{
		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->OnShutdown();
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
			components_.at(i)->OnUpdate();
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
			components_.at(i)->OnDrawInspector();
		}
	}
}
