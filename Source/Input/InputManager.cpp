#include "InputManager.h"

namespace argent::input
{
	void InputManager::OnAwake()
	{
		keyboard_.OnAwake();
	}

	void InputManager::OnShutdown()
	{
		Subsystem::OnShutdown();
	}

	void InputManager::Update(HWND hwnd)
	{
		keyboard_.Update();
		mouse_.Update(hwnd);

		for(int i = 0; i < 4; ++i)
		{
			pad_[i].Update();
		}
	}
}
