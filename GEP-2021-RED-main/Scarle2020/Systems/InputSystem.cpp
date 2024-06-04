#include "pch.h"
#include "InputSystem.h"

void InputSystem::init(ECS::ECS& ecs, HWND window)
{
    // Create Signiture
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<InputData>());
    psSig.set(ecs.getComponentBitflag<MoveData>());
    psSig.set(ecs.getComponentBitflag<ButtonData>());
    ecs.setSystemSignature<InputSystem>(psSig);

    // Create the Keyboard and Mouse
    //documentation here: https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input
    m_keyboard = std::make_unique<DirectX::Keyboard>();
    m_mouse = std::make_unique<DirectX::Mouse>();
    m_mouse->SetWindow(window);
    m_mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

    // Load keybinds
    loadKeyBinds();
}

void InputSystem::loadKeyBinds()
{
    // hardcoded for defaults but will later be saved to file
    m_p1_map.up = Keyboard::Keys::W;
    m_p1_map.down = Keyboard::Keys::S;
    m_p1_map.left = Keyboard::Keys::A;
    m_p1_map.right = Keyboard::Keys::D;

    m_p1_map.select = Keyboard::Keys::Enter;
    m_p1_map.back = Keyboard::Keys::Escape;
    m_p1_map.fire = Keyboard::Keys::Space;
    m_p1_map.power_up = Keyboard::Keys::E;

    m_p2_map.up = Keyboard::Keys::NumPad8;
    m_p2_map.down = Keyboard::Keys::NumPad5;
    m_p2_map.left = Keyboard::Keys::NumPad4;
    m_p2_map.right = Keyboard::Keys::NumPad6;

    m_p2_map.fire = Keyboard::Keys::NumPad0;
}

void InputSystem::readInput(ECS::ECS& ecs)
{
    // Read the input 
    m_mouse_state = m_mouse->GetState();
    m_keyboard_state = m_keyboard->GetState();
    m_keyboard_tracker.Update(m_keyboard_state);

    // Quit game on escape
    // if (m_keyboard_state.IsKeyDown(Keyboard::Escape))
    // {
    //     PostQuitMessage(0);
    // }

    
    
}

void InputSystem::applyPlayerInput(ECS::ECS& ecs)
{
    // Add Correct Input to Correct Entity
    for (const auto& entity : m_entities)
    {
        auto& key_data = ecs.getComponent<InputData>(entity);
        auto& move_data = ecs.getComponent<MoveData>(entity);
        auto& button_data = ecs.getComponent<ButtonData>(entity);

        setMoveData(key_data, move_data);
        setButtonData(key_data, button_data);
    }
}

void InputSystem::setMoveData(InputData& key_data, MoveData& move_data)
{
    bool up_key_pressed = m_keyboard_state.IsKeyDown(key_data.up);
    bool down_key_pressed = m_keyboard_state.IsKeyDown(key_data.down);
    bool left_key_pressed = m_keyboard_state.IsKeyDown(key_data.left);
    bool right_key_pressed = m_keyboard_state.IsKeyDown(key_data.right);

    move_data.direction.x = right_key_pressed ? 1.F : 0.F;
    move_data.direction.x -= left_key_pressed ? 1.F : 0.F;
    move_data.direction.y = down_key_pressed ? 1.F : 0.F;
    move_data.direction.y -= up_key_pressed ? 1.F : 0.F;

    move_data.direction.Normalize();
}

void InputSystem::setButtonData(InputData& key_data, ButtonData& button_data)
{
    button_data.select_button_down = m_keyboard_state.IsKeyDown(key_data.select);
    button_data.back_button_down = m_keyboard_state.IsKeyDown(key_data.back);
    button_data.fire_button_down = m_keyboard_state.IsKeyDown(key_data.fire);
    button_data.power_up_button_down = m_keyboard_state.IsKeyDown(key_data.power_up);
}

InputData& InputSystem::getBindings(const InputType& input_type)
{
    switch (input_type)
    {
    case InputType::PLAYER1:
        return m_p1_map;

    case InputType::PLAYER2:
        return m_p2_map;

    case InputType::UI:
        std::cout << "UI Binds Not Yet Implimented\n";
        break;

    case InputType::NONE:
        std::cout << "INPUT SYSTEM : No Bindings Selected in getMoveDataBindings(enum)\n";
        break;
    }

    return m_empty_map;
}

void InputSystem::updateBindings(InputData p1_map, InputData p2_map)
{
    m_p1_map = p1_map;
    m_p2_map = p2_map;
}

