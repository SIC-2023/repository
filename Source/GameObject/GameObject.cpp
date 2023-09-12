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
	* \brief is_activeがtrueになったときに一度だけ呼ばれる
	* 一番最初に呼ばれる
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
	 * \brief is_activeがtrueになるたびに呼ばれる
	 * Awake()の後で呼ばれる
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
	 * \brief Awake()と同じくis_active=trueのときに一度だけ呼ばれる
	 * Awake(), OnEnable() のあとに呼ばれる
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
	 * \brief 毎フレーム1回呼ばれる
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
	 * \brief is_activeがfalseになるたびに呼ばれる
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
	 * \brief ゲームオブジェクトが破棄されるときに呼ばれる
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
