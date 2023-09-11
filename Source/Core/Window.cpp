#include "Window.h"
#include "../Graphics/ImGuiController.h"

#include "Engine.h"

namespace argent
{
	LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;

		default:
			break;
		}

		//ImGui Window procedure
		graphics::ImGuiController::ImGuiWndProcHandler(hWnd, msg, wParam, lParam);

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Window::Window():
		window_width_(1280)
	,	window_height_(720)
	{}

	void Window::OnAwake()
	{
		//Creating and registering window
		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = HandleMessage;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = L"Window";

		if(!RegisterClassExW(&wc)) _ASSERT_EXPR(FALSE, L"Already registered");

		RECT rc{ 0, 0, window_width_, window_height_ };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		DWORD window_style = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME;

		if(GetEngine()->GetIsEditorMode())
		{
			window_style |= WS_MAXIMIZE;
		}

		hwnd_ = CreateWindowExW(0, wc.lpszClassName, L"Argent Engine",
			 window_style, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
			rc.bottom - rc.top, nullptr, nullptr, 
			wc.hInstance, nullptr);

		ShowWindow(hwnd_, SW_SHOW);
		Subsystem::OnAwake();
	}

	void Window::OnShutdown()
	{
		Subsystem::OnShutdown();
	}

	void Window::ProcessSystemEventQueue()
	{
		MSG msg{};

		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(WM_QUIT == msg.message) GetEngine()->RequestShutdown();
	}
}
