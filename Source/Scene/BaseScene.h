#pragma once

#include <string>

namespace argent::rendering
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
		virtual void Render(const rendering::RenderContext& render_context) = 0;

		//TODO virtual ����Ȃ����邱��
		virtual void DrawGui() = 0;

		const std::string& GetName() const { return name_; }
		const char* GetNameCStr() const { return name_.c_str(); }
	private:
		std::string name_;
	};
}

