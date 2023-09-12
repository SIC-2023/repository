#pragma once
#include "EditorWindow.h"

namespace argent
{
	class GameObject;
}

namespace argent::editor
{
	class InspectorWindow final :
			public EditorWindow
	{
	public:
		InspectorWindow();
		void Awake() override;
		void OnShutdown() override;
		void OnRender() override;

		//TODO
		static GameObject* focused_game_object_;
	};
}

