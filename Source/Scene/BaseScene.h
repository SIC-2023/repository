#pragma once

#include <string>

#include "../GameObject/GameObject.h"

namespace argent::rendering
{
	class RenderContext;
}
namespace argent::scene
{
	//TODO　仮置き　いずれエディタ上だけの存在にしたい
	class BaseScene
	{
	public:
		BaseScene(const char* name):
		name_(name)
		{}
		virtual ~BaseScene() = default;

		virtual void Awake();
		virtual void Shutdown();

		virtual void Update();
		virtual void Render(const rendering::RenderContext& render_context) = 0;

		void DrawGui();

		const std::string& GetName() const { return name_; }
		const char* GetNameCStr() const { return name_.c_str(); }


		GameObject* AddObject(std::string name);
	private:
		std::string name_;
		std::vector<std::unique_ptr<GameObject>> game_objects_;
	};
}

