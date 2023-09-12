#include "InputManager.h"

namespace argent::input
{
	void InputManager::Awake()
	{
		keyboard_.OnAwake();
	}

	void InputManager::Shutdown()
	{
		Subsystem::Shutdown();
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
