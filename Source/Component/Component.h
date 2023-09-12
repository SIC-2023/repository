#pragma once

#include <string>

namespace argent
{
	class GameObject;
}

namespace argent::component
{

	class Component
	{
	public:
		friend class argent::GameObject;
	public:
		Component(std::string name):
			name_(name) {}
		virtual ~Component() = default;

		virtual void Awake() {}
		virtual void OnEnable() {}
		virtual void Start() {}
		virtual void Update() {}
		virtual void OnDisable() {}
		virtual void OnDestroy() {}

		GameObject* GetOwner() const { return owner_; }
		std::string GetName() const { return name_; }

		void SetIsActive(bool active_flag);
		bool GetIsActive() const { return is_active_; }
	private:
		virtual void OnGui() {}
		void SetOwner(GameObject* owner) { owner_ = owner; }
	private:
		GameObject* owner_;
		std::string name_;
		bool is_active_ = true;
	};
}

