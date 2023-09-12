#pragma once

#include "EditorWindow.h"

namespace argent::editor
{
	class SceneWindow final
		: public EditorWindow
	{
	public:
		SceneWindow();

		void Awake() override;
		void OnShutdown() override;
		void OnRender() override;
	private:
	};
}

