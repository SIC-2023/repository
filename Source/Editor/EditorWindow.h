#pragma once

#include <string>

namespace argent::editor
{
	class EditorWindow
	{
	public:
		EditorWindow(std::string name);
		virtual ~EditorWindow() = default;

		virtual void OnAwake() {}
		virtual void OnShutdown() {}

		virtual void OnRender() = 0;

		std::string GetName() const { return window_name_; }

	private:
		std::string window_name_;
	};
}

