#include "GameObject.h"

#include "imgui.h"

namespace argent
{
	GameObject::GameObject(std::string name):
		name_(name)
	{
		transform_ = AddComponent<component::Transform>();
	}

	/**
	* \brief is_active��true�ɂȂ����Ƃ��Ɉ�x�����Ă΂��
	* ��ԍŏ��ɌĂ΂��
	*/
	void GameObject::Awake() const
	{
		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			if(child_objects_.at(i)->GetIsActive()) child_objects_.at(i)->Awake();
		}

		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->Awake();
		}
	}

	/**
	 * \brief is_active��true�ɂȂ邽�тɌĂ΂��
	 * Awake()�̌�ŌĂ΂��
	 */
	void GameObject::OnEnable() const
	{
		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			if(child_objects_.at(i)->GetIsActive()) child_objects_.at(i)->OnEnable();
		}

		for(size_t i = 0; i < components_.size(); ++i)
		{
			if(components_.at(i)->GetIsActive()) components_.at(i)->OnEnable();
		}
	}

	/**
	 * \brief Awake()�Ɠ�����is_active=true�̂Ƃ��Ɉ�x�����Ă΂��
	 * Awake(), OnEnable() �̂��ƂɌĂ΂��
	 */
	void GameObject::Start() const
	{
		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			if(child_objects_.at(i)->GetIsActive()) child_objects_.at(i)->Start();
		}

		for(size_t i = 0; i < components_.size(); ++i)
		{
			if(components_.at(i)->GetIsActive()) components_.at(i)->Start();
		}
	}

	/**
	 * \brief ���t���[��1��Ă΂��
	 */
	void GameObject::Update() const
	{
		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			if(child_objects_.at(i)->GetIsActive()) child_objects_.at(i)->Update();
		}

		for(size_t i = 0; i < components_.size(); ++i)
		{
			if(components_.at(i)->GetIsActive()) components_.at(i)->Update();
		}
	}

	/**
	 * \brief is_active��false�ɂȂ邽�тɌĂ΂��
	 */
	void GameObject::OnDisable() const
	{
		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			if(child_objects_.at(i)->GetIsActive()) child_objects_.at(i)->OnDisable();
		}

		for(size_t i = 0; i < components_.size(); ++i)
		{
			if(components_.at(i)->GetIsActive()) components_.at(i)->OnDisable();
		}
	}

	/**
	 * \brief �Q�[���I�u�W�F�N�g���j�������Ƃ��ɌĂ΂��
	 */
	void GameObject::OnDestroy() const
	{
		for(size_t i = 0; i < child_objects_.size(); ++i)
		{
			if(child_objects_.at(i)->GetIsActive()) child_objects_.at(i)->OnDestroy();
		}

		for(size_t i = 0; i < components_.size(); ++i)
		{
			components_.at(i)->OnDestroy();
		}
	}

	void GameObject::SetIsActive(bool active_flag)
	{
		if(is_active_ != active_flag)
		{
			is_active_ ? OnDisable() : OnEnable();
		}
		is_active_ = active_flag;
	}

	void GameObject::OnGui() const
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
