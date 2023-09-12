#pragma once

#include "../SubSystem/Subsystem.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "GamePad.h"

namespace argent::input
{
	class InputManager final : public Subsystem
	{
	public:

		InputManager() = default;
		~InputManager() override = default;

		void Awake() override;
		void Shutdown() override;

		void Update(HWND hwnd);
		const Keyboard& GetKeyboard() const { return keyboard_; }
		const Mouse& GetMouse() const { return mouse_; }
		const GamePad& GetGamePad(int index) const { return pad_[index]; }
	private:

		Keyboard keyboard_;
		Mouse mouse_;
		GamePad pad_[3];
	};
}

