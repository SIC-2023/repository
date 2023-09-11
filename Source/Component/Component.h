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

		virtual void Start() {}
		virtual void OnAwake() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate() {}
		virtual void OnDrawInspector() {}

		GameObject* GetOwner() const { return owner_; }
		std::string GetName() const { return name_; }

	private:
		void SetOwner(GameObject* owner) { owner_ = owner; }
	private:
		GameObject* owner_;
		std::string name_;
	};
}

