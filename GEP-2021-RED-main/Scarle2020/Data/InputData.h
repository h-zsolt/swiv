#pragma once
#include "pch.h"
#include "Keyboard.h"

enum struct InputType
{
	NONE,
	PLAYER1,
	PLAYER2,
	UI
};

using Axis = float;

// Keybinds
struct InputData
{
	InputData() = default;
	InputData(const InputData&) = default;

	using KeyCode = DirectX::Keyboard::Keys;
	// Movement Keys
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;

	// Action Keys
	KeyCode select;
	KeyCode back;
	KeyCode fire;
	KeyCode power_up;

	std::array<KeyCode, 8> m_bindings = {up, down, left, right, select, back, fire, power_up};
	void updateBindingsArray() { m_bindings = { up, down, left, right, select, back, fire, power_up }; }
};

