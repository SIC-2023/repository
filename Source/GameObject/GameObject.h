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

	class GameObject
	{
	public:

	public:
		//Á‚¹
		GameObject(std::string name);

		/**
		 * \brief â‘Î‚É1‰ñ‚µ‚©ŒÄ‚Î‚ê‚È‚¢ŠÖ”
		 */
		void Start();
		void Awake();
		void OnShutDown();
		void OnUpdate();

		void OnDrawInspector();

		template<class T>
		T* AddComponent();

		GameObject* AddChild();

		template<class T>
		T* GetComponent()  const;

		size_t GetChildCounts() const { return child_objects_.size(); }
		GameObject* GetChild(size_t child_index) const { return child_objects_.at(child_index).get(); }

		size_t GetComponentsCount() const { return components_.size(); }
		std::string GetName() const { return name_; }
		component::Transform* GetTransform() const { return transform_; }
	private:
		std::string name_;
		Tag tag_;
		component::Transform* transform_ = nullptr;
		std::vector<std::unique_ptr<component::Component>> components_;
		std::vector<std::unique_ptr<GameObject>> child_objects_;
	};

	template <class T>
	T* GameObject::AddComponent()
	{
		static_assert(std::is_base_of<component::Component, T>::value, "Component‚Å‚Í‚È‚¢‚Å‚·");
		components_.emplace_back(std::make_unique<T>());
		auto* com = (*components_.rbegin()).get();
		com->SetOwner(this);
		return reinterpret_cast<T*>(com);
	}

	template <class T>
	T* GameObject::GetComponent() const
	{
		static_assert(std::is_base_of<component::Component, T>::value, "Component‚Å‚Í‚È‚¢‚Å‚·");
		for(auto& com : components_)
		{
			if(typeid(com) != typeid(T)) continue;

			return com.get();
		}

		return nullptr;
	}
}

