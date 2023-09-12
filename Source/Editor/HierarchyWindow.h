#pragma once

#include "EditorWindow.h"

namespace argent::editor
{
	class HierarchyWindow final :
		public EditorWindow
	{
	public:

		HierarchyWindow();
		void Awake() override;
		void OnShutdown() override;
		void OnRender() override;

		
	};
}

