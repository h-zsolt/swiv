#include "pch.h"
#include <Components/TransformComponent.h> 
#include <Components/SpriteComponent.h>   
#include <Systems/InputSystem.h>

#include "ECS.h"
#include <Game/Game.h>
#include "InMenu.h"
#include "InGame.h"
#include "InControls.h"
#include <Audio/AudioManager.cpp>

void InMenu::start()
{
	// Start
	m_start_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/start.dds", m_start_entity, { 960.F,100.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 1.0f);

	// Controls
	m_controls_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/controls.dds", m_controls_entity, { 960.F,300.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 1.0f);

	// Player count select
	m_players_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/1and2player.dds", m_players_entity, { 960.F,500.f }, { 30.f,89.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 1.0f);

	const float thumb_scale = 0.23F;
	// Level1 thumb
	m_level1_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/level1thumb.dds", m_level1_entity, { 960.F, 700.F }, { 0.F, 0.F }, { thumb_scale, thumb_scale }, { 1.F, 1.F, 1.F, 1.F }, 1.0F);

	// Level2 thumb
	m_level2_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/level2thumb.dds", m_level2_entity, { 960.F, 700.F }, { 0.F, 0.F }, { thumb_scale, thumb_scale }, { 1.F, 1.F, 1.F, 1.F }, 1.0F, false);

	// Level3 thumb
	m_level3_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/level3thumb.dds", m_level3_entity, { 960.F, 700.F }, { 0.F, 0.F }, { thumb_scale, thumb_scale }, { 1.F, 1.F, 1.F, 1.F }, 1.0F, false);

	// Level4 thumb
	m_level4_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/level4thumb.dds", m_level4_entity, { 960.F, 700.F }, { 0.F, 0.F }, { thumb_scale, thumb_scale }, { 1.F, 1.F, 1.F, 1.F }, 1.0F, false);

	// Exit
	m_exit_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/exit.dds", m_exit_entity, { 960.F,900.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 1.0f);

	// Highlight
	m_select_entity = m_game->ecs()->createEntity();
	loadSprite("Assets/select.dds", m_select_entity, { 960.F,300.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 0.3F }, 0.0f);

	SpriteComponent& player_sprite = m_game->ecs()->getComponent<SpriteComponent>(m_players_entity);
	player_sprite.m_source_rect = { 0,0,203,75 };
}

void InMenu::end()
{
	auto* p_ecs = m_game->ecs();

	p_ecs->destroyEntity(m_start_entity);
	p_ecs->destroyEntity(m_controls_entity);
	p_ecs->destroyEntity(m_exit_entity);
	p_ecs->destroyEntity(m_select_entity);
	p_ecs->destroyEntity(m_players_entity);
	p_ecs->destroyEntity(m_level1_entity);
	p_ecs->destroyEntity(m_level2_entity);
	p_ecs->destroyEntity(m_level3_entity);
	p_ecs->destroyEntity(m_level4_entity);


	m_game->setNumSelectedPlayers(players);
	m_game->setLevelSelected(static_cast<uint8_t>(m_selected_level));
}

void InMenu::unpaused()
{
	start();
}

bool InMenu::loadSprite(const std::string& texture_path,
	ECS::Entity entity,
	const std::pair<float, float>& transform,
	const std::pair<float, float>& relative_pos,
	const std::pair<float, float>& scale,
	const DirectX::SimpleMath::Color& color,
	float z_order,
	bool visible)
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
		sprite.m_render = visible;
		m_game->ecs()->addComponent(entity, sprite);
	}
	catch (...)
	{
		std::cout << "error loading texture" << std::endl;
		return false;
	}
	return true;
}

bool InMenu::loadNextScreen()
{
	switch (m_item_selected)
	{
	case MenuItems::START:
	{
		// load game
		playMenuSound();
		m_game->setState(std::make_unique<InGame>());
		return true;
	}
	case MenuItems::CONTROLS:
	{
		// load controls screen
		playMenuSound();
		end();
		m_game->pauseState(std::make_unique<InControls>());
		return true;
	}
	case MenuItems::PLAYERS:
	{
		// set number of players
		if (players == 0)
		{
			players = 1;
			SpriteComponent& sprite = m_game->ecs()->getComponent<SpriteComponent>(m_players_entity);
			sprite.m_source_rect = { 0,75,203,150 };
		}
		else if (players == 1)
		{
			players = 0;
			SpriteComponent& sprite = m_game->ecs()->getComponent<SpriteComponent>(m_players_entity);
			sprite.m_source_rect = { 0,0,203,75 };
		}
		break;
	}
	case MenuItems::LEVELS:
	{
		switch (m_selected_level)
		{
			case Levels::LEVEL1:
			{
				m_selected_level = Levels::LEVEL2;
				switchLevelThumb(m_level1_entity, m_level2_entity);
				break;
			}


			case Levels::LEVEL2:
			{
				m_selected_level = Levels::LEVEL3;
				switchLevelThumb(m_level2_entity, m_level3_entity);
				break;
			}

			case Levels::LEVEL3:
			{
				m_selected_level = Levels::LEVEL4;
				switchLevelThumb(m_level3_entity, m_level4_entity);
				break;
			}
			case Levels::LEVEL4:
			{
				m_selected_level = Levels::LEVEL1;
				switchLevelThumb(m_level4_entity, m_level1_entity);
				break;
			}
		}
		break;
	}
	case MenuItems::EXIT:
	{
		// exit game
		PostQuitMessage(0);
		break;
	}
	}
	return false;
}

void InMenu::playMenuSound()
{
	const auto i = AudioManager::loadSoundEffect("Assets/ui_browse.wav");
	auto* inst = AudioManager::createEffectInstance(i);
	AudioManager::playSound(inst, 0.5F);
}

void InMenu::updateObjects(GameData* gd)
{
	if (getKeyboardData())
	{
		return;
	}

	
	TransformComponent& transform = m_game->ecs()->getComponent<TransformComponent>(m_select_entity);
	transform.m_scale = { 1.f, 1.f };

	switch (m_item_selected)
	{
	case MenuItems::START:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_start_entity);
		transform.m_position = position.m_position;
		break;
	}
	case MenuItems::CONTROLS:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_controls_entity);
		transform.m_position = position.m_position;
		break;
	}
	case MenuItems::PLAYERS:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_players_entity);
		transform.m_position = position.m_position;
		break;
	}
	case MenuItems::LEVELS:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_level1_entity);
		transform.m_position = position.m_position;
		transform.m_scale = { 2.f, 3.75f };
		break;
	}
	case MenuItems::EXIT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_exit_entity);
		transform.m_position = position.m_position;
		break;
	}
	}
}

bool InMenu::getKeyboardData()
{
	const auto& m_keyboard_state = m_game->getInputSystem()->keyboardState();
	auto* input_system = m_game->getInputSystem();
	if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::Enter))
	{
		playMenuSound();
		return loadNextScreen();
	}
	if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::W))
	{
		m_item_selected = incrementItem(m_item_selected, -1);
		playMenuSound();
	}
	else if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::S))
	{
		m_item_selected = incrementItem(m_item_selected, 1);
		playMenuSound();
	}
	return false;
}

InMenu::MenuItems InMenu::incrementItem(MenuItems current_item, int i)
{
	auto item = static_cast<int>(current_item) + i;
	if (item >= 5)
	{
		item = 0;
	}
	else if (item < 0)
	{
		item = 4;
	}
	return static_cast<MenuItems>(item);
}

void InMenu::switchLevelThumb(ECS::Entity from, ECS::Entity to) noexcept
{
	SpriteComponent& from_sprite = m_game->ecs()->getComponent<SpriteComponent>(from);
	from_sprite.m_render = false;
	SpriteComponent& to_sprite = m_game->ecs()->getComponent<SpriteComponent>(to);
	to_sprite.m_render = true;
}
