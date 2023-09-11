#pragma once

#include "EditorWindow.h"

namespace argent::editor
{
	class ProjectWindow final :
		public EditorWindow
	{
	public:
		ProjectWindow();

		void OnAwake() override;
		void OnShutdown() override;
		void OnRender() override;

	private:
	};
}

