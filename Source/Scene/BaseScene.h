#pragma once

#include <string>

namespace argent::graphics
{
	class RenderContext;
}
namespace argent::scene
{
	//TODO�@���u���@������G�f�B�^�ゾ���̑��݂ɂ�����
	class BaseScene
	{
	public:
		BaseScene(const char* name):
		name_(name)
		{}
		virtual ~BaseScene() = default;

		virtual void OnAwake() {}
		virtual void OnShutdown() {}


		virtual void Update() = 0;
		virtual void Render(const graphics::RenderContext& render_context) = 0;

		const std::string& GetName() const { return name_; }
		const char* GetNameCStr() const { return name_.c_str(); }
	private:
		std::string name_;
	};
}

