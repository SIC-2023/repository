#pragma once
#include <Windows.h>

#include "../SubSystem/Subsystem.h"

namespace argent
{
	class Window final:
		public Subsystem
	{
	public:
		Window();
		virtual ~Window() = default;

		void OnAwake() override;
		void OnShutdown() override;

		HWND GetHwnd() const { return hwnd_; }
		LONG GetWindowWidth() const { return window_width_; }
		LONG GetWindowHeight() const { return window_height_; }

		//message peek‚Æ‚©
		void ProcessSystemEventQueue();

	private:
		HWND hwnd_{};
		LONG window_width_;
		LONG window_height_;
	};
}

