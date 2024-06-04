#pragma once

#include "pch.h"
#include "ECS.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <Data/InputData.h>
#include <Data/MoveData.h>
#include <Data/ButtonData.h>

class InputSystem : public ECS::System
{
	using Keyboard = DirectX::Keyboard;
	using Mouse = DirectX::Mouse;

public:
	void init(ECS::ECS& ecs, HWND window);
	void loadKeyBinds();

	void readInput(ECS::ECS& ecs);
	void setMoveData(InputData& input_data, MoveData& move_data);
	void setButtonData(InputData& input_data, ButtonData& button_data);
	void applyPlayerInput(ECS::ECS& ecs);

	InputData& getBindings(const InputType& input_type);

	const Mouse::State& mouseState() const { return m_mouse_state; }
	const Keyboard::State& keyboardState() const { return m_keyboard_state; }
	const Keyboard::KeyboardStateTracker& keyboardStateTracker() const { return m_keyboard_tracker; }
	Mouse* mouse() { return m_mouse.get(); }

	void updateBindings(InputData p1_map, InputData p2_map);


private:
	// Keyboard & Mouse members
	std::unique_ptr<Keyboard> m_keyboard = nullptr;
	std::unique_ptr<Mouse>    m_mouse = nullptr;

	Mouse::State    m_mouse_state;
	Keyboard::State m_keyboard_state;
	Keyboard::KeyboardStateTracker m_keyboard_tracker;

	InputData m_p1_map = {};
	InputData m_p2_map = {};
	InputData m_empty_map = {};
};