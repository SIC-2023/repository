#pragma once
#include "EditorWindow.h"

namespace argent::editor
{
	class InspectorWindow final :
			public EditorWindow
	{
	public:
		InspectorWindow();
		void OnAwake() override;
		void OnShutdown() override;
		void OnRender() override;
	};
}

