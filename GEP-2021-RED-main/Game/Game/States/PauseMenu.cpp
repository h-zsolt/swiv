#include "pch.h"
#include <Components/TransformComponent.h> 
#include <Components/SpriteComponent.h>   
#include <Systems/InputSystem.h>
#include <Components/TextRenderComponent.h>
#include <Audio/AudioManager.hpp>

#include "PauseMenu.h"
#include "InControls.h"
#include "ECS.h"
#include <Game/Game.h>

void PauseMenu::start()
{
	// m_game->m_paused = true;
	auto& cam_pos = m_game->getGameRenderer()->getGameCameraPosition();
	auto entity = m_entities.emplace_back(m_game->ecs()->createEntity());
	loadSprite("Assets/controls.dds", entity, { 960.f, -cam_pos.y + 500 }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, -1.f);
	entity = m_entities.emplace_back(m_game->ecs()->createEntity());
	loadSprite("Assets/exit.dds", entity, { 960.f, -cam_pos.y + 700.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, -1.f);
	entity = m_entities.emplace_back(m_game->ecs()->createEntity());
	loadSprite("Assets/select.dds", entity, { 960.f,500.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 0.3F }, -1.f);

}

void PauseMenu::updateObjects(GameData* gd)
{
	
	std::cout << "in pause menu" << std::endl;
	TransformComponent& transform = m_game->ecs()->getComponent<TransformComponent>(m_entities.back());
	switch (m_selected_option)
	{
	case MenuOption::CONTROLS:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[0]);
		transform.m_position = position.m_position;
		break;
	}
	case MenuOption::EXIT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[1]);
		transform.m_position = position.m_position;
		break;
	}
	}
	keyboardInput();
}

bool PauseMenu::loadSprite(const std::string& texture_path, ECS::Entity entity, const std::pair<float, float>& transform, const std::pair<float, float>& relative_pos, const std::pair<float, float>& scale, const DirectX::SimpleMath::Color& color, float z_order)
{
	try
	{
		auto id = m_game->getGameRenderer()->loadTexture(m_game->getDevice(), texture_path);
		TransformComponent ecs_transform;
		ecs_transform.m_position = { transform.first, transform.second };
		ecs_transform.m_scale = { scale.first, scale.second };
		m_game->ecs()->addComponent(entity, ecs_transform);
		SpriteComponent sprite;
		m_game->getGameRenderer()->setupSpriteComponent(id, sprite);
		sprite.m_relative_position = { relative_pos.first, relative_pos.second };
		sprite.m_colour = color;
		sprite.m_z_order = z_order;
		m_game->ecs()->addComponent(entity, sprite);
	}
	catch (...)
	{
		std::cout << "error loading texture" << std::endl;
		return false;
	}
	return true;
}

void PauseMenu::keyboardInput()
{
	const auto& m_keyboard_state = m_game->getInputSystem()->keyboardState();
	auto* input_system = m_game->getInputSystem();
	if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::Keys::Escape))
	{
		m_game->unpauseState();
	}
	if (m_keyboard_state.IsKeyDown(DirectX::Keyboard::Keys::Enter) && !m_enter_down)
	{
		m_enter_down = true;
		selectOption();
	}
	else if (m_keyboard_state.IsKeyDown(DirectX::Keyboard::Keys::W) && !m_w_down)
	{
		m_w_down = true;
		m_selected_option = static_cast<MenuOption>(1 - static_cast<int>(m_selected_option));
	}
	else if (m_keyboard_state.IsKeyDown(DirectX::Keyboard::Keys::S) && !m_s_down)
	{
		m_s_down = true;
		m_selected_option = static_cast<MenuOption>(1 - static_cast<int>(m_selected_option));
	}
	if (m_keyboard_state.IsKeyUp(DirectX::Keyboard::Keys::W))
	{
		m_w_down = false;
	}
	if (m_keyboard_state.IsKeyUp(DirectX::Keyboard::Keys::S))
	{
		m_s_down = false;
	}
	if (m_keyboard_state.IsKeyUp(DirectX::Keyboard::Keys::Enter))
	{
		m_enter_down = false;
	}
}

void PauseMenu::selectOption()
{
	switch (m_selected_option)
	{
	case MenuOption::CONTROLS:
	{
		m_game->setState(std::make_unique<InControls>(),false,false,false);
		break;
	}
	case MenuOption::EXIT:
	{
		m_game->clearPauseState();
		m_game->setState(std::make_unique<InMenu>());
		break;
	}
	}
}

void PauseMenu::end()
{
	for (auto& ent : m_entities)
	{
		m_game->ecs()->destroyEntity(ent);
	}
	// m_game->m_paused = false;
}
