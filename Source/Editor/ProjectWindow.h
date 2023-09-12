#pragma once

#include "EditorWindow.h"

namespace argent::editor
{
	class ProjectWindow final :
		public EditorWindow
	{
	public:
		ProjectWindow();

		void Awake() override;
		void OnShutdown() override;
		void OnRender() override;

	private:
	};
}

