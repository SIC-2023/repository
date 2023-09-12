#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../Component/Component.h"
#include "../Component/Transform.h"

namespace argent
{
	using Tag = uint64_t;
	namespace tag
	{
		static constexpr Tag None =		0x01;
		static constexpr Tag Camera =	0x01 << 1;
		static constexpr Tag Light =	0x01 << 2;

	}

	/**
	 * \brief �V�[����ɑ��݂��邷�ׂẴI�u�W�F�N�g�̊��N���X
	 * ���̂Ƃ���p���������͂Ȃ�
	 */
	class GameObject
	{
	public:

	public:
		//TODO ����
		GameObject(std::string name);

		/**
		 * \brief is_active��true�ɂȂ����Ƃ��Ɉ�x�����Ă΂��
		 * ��ԍŏ��ɌĂ΂��
		 */
		void Awake() const;

		/**
		 * \brief is_active��true�ɂȂ邽�тɌĂ΂��
		 * Awake()�̌�ŌĂ΂��
		 */
		void OnEnable() const;

		/**
		 * \brief Awake()�Ɠ�����is_active=true�̂Ƃ��Ɉ�x�����Ă΂��
		 * Awake(), OnEnable() �̂��ƂɌĂ΂��
		 */
		void Start() const;

		/**
		 * \brief ���t���[��1��Ă΂��
		 */
		void Update() const;

		/**
		 * \brief is_active��false�ɂȂ邽�тɌĂ΂��
		 */
		void OnDisable() const;

		/**
		 * \brief �Q�[���I�u�W�F�N�g���j�������Ƃ��ɌĂ΂��
		 */
		void OnDestroy() const;

		void OnGui() const;

		template<class T>
		T* AddComponent();

		GameObject* AddChild();

		template<class T>
		T* GetComponent()  const;

		size_t GetChildCounts() const { return child_objects_.size(); }
		GameObject* GetChild(size_t child_index) const { return child_objects_.at(child_index).get(); }

		size_t GetComponentCounts() const { return components_.size(); }
		std::string GetName() const { return name_; }
		component::Transform* GetTransform() const { return transform_; }
		bool GetIsActive() const { return is_active_; }
		void SetIsActive(bool active_flag);
	private:
		std::string name_;
		Tag tag_;
		bool is_active_ = true;
		component::Transform* transform_ = nullptr;
		std::vector<std::unique_ptr<component::Component>> components_;
		std::vector<std::unique_ptr<GameObject>> child_objects_;
	};

	template <class T>
	T* GameObject::AddComponent()
	{
		static_assert(std::is_base_of<component::Component, T>::value, "Component�ł͂Ȃ��ł�");
		components_.emplace_back(std::make_unique<T>());
		auto* com = (*components_.rbegin()).get();
		com->SetOwner(this);
		return reinterpret_cast<T*>(com);
	}

	template <class T>
	T* GameObject::GetComponent() const
	{
		static_assert(std::is_base_of<component::Component, T>::value, "Component�ł͂Ȃ��ł�");
		for(auto& com : components_)
		{
			if(typeid(com) != typeid(T)) continue;

			return com.get();
		}

		return nullptr;
	}
}

