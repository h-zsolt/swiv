#include "pch.h"
#include <Components/TransformComponent.h> 
#include <Components/SpriteComponent.h>   
#include <Systems/InputSystem.h>
#include <Components/TextRenderComponent.h>
#include <Audio/AudioManager.hpp>

#include "InControls.h"
#include <Data/InputData.h>
#include "ECS.h"
#include <Game/Game.h>

void InControls::start()
{

	auto& cam_pos = m_game->getGameRenderer()->getGameCameraPosition();

	auto id = m_game->getGameRenderer()->loadFont(m_game->getDevice(), "Assets/arial.spritefont");
	TextRenderComponent text;
	text.m_relative_position = { 120.f, -30.f };
	text.m_relative_scale = { 0.5f, 0.5f };
	text.m_font_id = id;

	auto entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::NO_ID)).first;
	loadSprite("Assets/back.dds", entity, { 960.f,-cam_pos.y + 200.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back( std::make_pair(m_game->ecs()->createEntity(),KeyBindingsSelector::P1_UP) ).first;
	loadSprite("Assets/p1_up.dds", entity, { 760.f,-cam_pos.y + 300.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P1_DOWN)).first;
	loadSprite("Assets/p1_down.dds", entity, { 760.f,-cam_pos.y + 400.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P1_LEFT)).first;
	loadSprite("Assets/p1_left.dds", entity, { 760.f,-cam_pos.y + 500.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P1_RIGHT)).first;
	loadSprite("Assets/p1_right.dds", entity, { 760.f,-cam_pos.y + 600.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P1_SELECT)).first;
	loadSprite("Assets/p1_select.dds", entity, { 760.f,-cam_pos.y + 700.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P1_BACK)).first;
	loadSprite("Assets/p1_back.dds", entity, { 760.f,-cam_pos.y + 800.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P1_FIRE)).first;
	loadSprite("Assets/p1_fire.dds", entity, { 760.f,-cam_pos.y + 900.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P1_POWERUP)).first;
	loadSprite("Assets/p1_powerup.dds", entity, { 760.f,-cam_pos.y + 1000.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity() , KeyBindingsSelector::P2_UP)).first;
	loadSprite("Assets/p2_up.dds", entity, { 1160.f,-cam_pos.y + 300.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity() , KeyBindingsSelector::P2_DOWN)).first;
	loadSprite("Assets/p2_down.dds", entity, { 1160.f,-cam_pos.y + 400.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity() , KeyBindingsSelector::P2_LEFT)).first;
	loadSprite("Assets/p2_left.dds", entity, { 1160.f,-cam_pos.y + 500.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P2_RIGHT)).first;
	loadSprite("Assets/p2_right.dds", entity, { 1160.f,-cam_pos.y + 600.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P2_SELECT)).first;
	loadSprite("Assets/p2_select.dds", entity, { 1160.f,-cam_pos.y + 700.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P2_BACK)).first;
	loadSprite("Assets/p2_back.dds", entity, { 1160.f,-cam_pos.y + 800.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P2_FIRE)).first;
	loadSprite("Assets/p2_fire.dds", entity, { 1160.f,-cam_pos.y + 900.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::P2_POWERUP)).first;
	loadSprite("Assets/p2_powerup.dds", entity, { 1160.f,-cam_pos.y + 1000.f }, { 0.f,0.f }, { 1.f,1.f }, { 1.F, 1.F, 1.F, 1.F }, 0.f);

	for (auto& ent : m_entities)
	{
		if (ent.second != KeyBindingsSelector::NO_ID)
		{
			m_game->ecs()->addComponent(ent.first, text);
		}
	}
	
	entity = m_entities.emplace_back(std::make_pair(m_game->ecs()->createEntity(), KeyBindingsSelector::NO_ID)).first;
	loadSprite("Assets/key_selector.dds", entity, { 760.f,-cam_pos.y + 300.f }, { 0.f,-2.f }, { 0.92f,0.77f }, { 1.F, 1.F, 1.F, 0.3F }, 0.f);

}

void InControls::updateObjects(GameData* gd)
{
	auto& m_p1_map = m_game->getInputSystem()->getBindings(InputType::PLAYER1);
	auto& m_p2_map = m_game->getInputSystem()->getBindings(InputType::PLAYER2);
	for (auto& ent : m_entities)
	{
		if (ent.second == KeyBindingsSelector::NO_ID)
		{
			continue;
		}
		TextRenderComponent& text = m_game->ecs()->getComponent<TextRenderComponent>(ent.first);
		switch (ent.second)
		{
		case KeyBindingsSelector::BACK:
		{
			break;
		}
		case KeyBindingsSelector::P1_UP:
		{
			text.m_text = findKeyInKeys(m_p1_map.up);
			break;
		}
		case KeyBindingsSelector::P1_DOWN:
		{
			text.m_text = findKeyInKeys(m_p1_map.down);
			break;
		}
		case KeyBindingsSelector::P1_LEFT:
		{
			text.m_text = findKeyInKeys(m_p1_map.left);
			break;
		}
		case KeyBindingsSelector::P1_RIGHT:
		{
			text.m_text = findKeyInKeys(m_p1_map.right);
			break;
		}
		case KeyBindingsSelector::P1_SELECT:
		{
			text.m_text = findKeyInKeys(m_p1_map.select);
			break;
		}
		case KeyBindingsSelector::P1_BACK:
		{
			text.m_text = findKeyInKeys(m_p1_map.back);
			break;
		}
		case KeyBindingsSelector::P1_FIRE:
		{
			text.m_text = findKeyInKeys(m_p1_map.fire);
			break;
		}
		case KeyBindingsSelector::P1_POWERUP:
		{
			text.m_text = findKeyInKeys(m_p1_map.power_up);
			break;
		}
		case KeyBindingsSelector::P2_UP:
		{
			text.m_text = findKeyInKeys(m_p2_map.up);
			break;
		}
		case KeyBindingsSelector::P2_DOWN:
		{
			text.m_text = findKeyInKeys(m_p2_map.down);
			break;
		}
		case KeyBindingsSelector::P2_LEFT:
		{
			text.m_text = findKeyInKeys(m_p2_map.left);
			break;
		}
		case KeyBindingsSelector::P2_RIGHT:
		{
			text.m_text = findKeyInKeys(m_p2_map.right);
			break;
		}
		case KeyBindingsSelector::P2_SELECT:
		{
			text.m_text = findKeyInKeys(m_p2_map.select);
			break;
		}
		case KeyBindingsSelector::P2_BACK:
		{
			text.m_text = findKeyInKeys(m_p2_map.back);
			break;
		}
		case KeyBindingsSelector::P2_FIRE:
		{
			text.m_text = findKeyInKeys(m_p2_map.fire);
			break;
		}
		case KeyBindingsSelector::P2_POWERUP:
		{
			text.m_text = findKeyInKeys(m_p2_map.power_up);
			break;
		}
		}
		if (ent.second == m_selected_binding && m_changing_key)
		{
			text.m_text = "___";
		}
	}
	TransformComponent& transform = m_game->ecs()->getComponent<TransformComponent>(m_entities.back().first);
	auto& cam_pos = m_game->getGameRenderer()->getGameCameraPosition();
	switch (m_selected_binding)
	{
	case KeyBindingsSelector::BACK:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[0].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_UP:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[1].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_DOWN:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[2].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_LEFT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[3].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_RIGHT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[4].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_SELECT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[5].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_BACK:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[6].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_FIRE:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[7].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P1_POWERUP:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[8].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_UP:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[9].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_DOWN:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[10].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_LEFT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[11].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_RIGHT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[12].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_SELECT:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[13].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_BACK:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[14].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_FIRE:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[15].first);
		transform.m_position = position.m_position;
		break;
	}
	case KeyBindingsSelector::P2_POWERUP:
	{
		TransformComponent& position = m_game->ecs()->getComponent<TransformComponent>(m_entities[16].first);
		transform.m_position = position.m_position;
		break;
	}
	}
	if (!m_changing_key)
	{
		selectBindings();
	}
	else
	{
		changeBinding();
	}
}

void InControls::selectBindings()
{
	const auto& m_keyboard_state = m_game->getInputSystem()->keyboardState();
	auto* input_system = m_game->getInputSystem();
	if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::Enter))
	{
		if (m_selected_binding == KeyBindingsSelector::BACK)
		{
			playMenuSound();
			m_game->unpauseState();
			return;
		}
		else
		{
			playMenuSound();
			m_changing_key = true;
			changeBinding();
		}
	}
	if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::W))
	{
		if (m_selected_binding == KeyBindingsSelector::P2_UP)
		{
			m_selected_binding = KeyBindingsSelector::BACK;
		}
		else
		{
			m_selected_binding = changeSelected(m_selected_binding, -1);
		}
		playMenuSound();
	}
	else if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::S))
	{
		m_selected_binding = changeSelected(m_selected_binding, 1);
		playMenuSound();
	}
	else if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::A) && m_selected_binding != KeyBindingsSelector::BACK)
	{
		if ((static_cast<int>(m_selected_binding) - 8) > 0)
		{
			m_selected_binding = changeSelected(m_selected_binding, -8);
			playMenuSound();
		}
	}
	else if (input_system->keyboardStateTracker().IsKeyPressed(DirectX::Keyboard::D) && m_selected_binding != KeyBindingsSelector::BACK)
	{
		if ((static_cast<int>(m_selected_binding) + 8) < 17)
		{
			m_selected_binding = changeSelected(m_selected_binding, 8);
			playMenuSound();
		}
	}
}

void InControls::changeBinding()
{
	const auto& m_keyboard_state = m_game->getInputSystem()->keyboardState();
	for (auto& key : m_keys)
	{
		if (m_keyboard_state.IsKeyDown(key.first))
		{
			auto& m_p1_map = m_game->getInputSystem()->getBindings(InputType::PLAYER1);
			auto& m_p2_map = m_game->getInputSystem()->getBindings(InputType::PLAYER2);
			m_p1_map.updateBindingsArray();
			m_p2_map.updateBindingsArray();
			for (uint8_t i = 0; i < m_p1_map.m_bindings.size(); i++)
			{
				if (m_p1_map.m_bindings[i] == key.first)
				{
					setBinding(DirectX::Keyboard::Keys::None, static_cast<KeyBindingsSelector>(i + 1));
				}
			}
			for (uint8_t i = 0; i < m_p2_map.m_bindings.size(); i++)
			{
				if (m_p2_map.m_bindings[i] == key.first)
				{
					setBinding(DirectX::Keyboard::Keys::None, static_cast<KeyBindingsSelector>(i + 9));
				}
			}
			setBinding(key.first,m_selected_binding);
			m_changing_key = false;
			return;
		}
	}
}

void InControls::setBinding(DirectX::Keyboard::Keys key, KeyBindingsSelector binding)
{
	auto& m_p1_map = m_game->getInputSystem()->getBindings(InputType::PLAYER1);
	auto& m_p2_map = m_game->getInputSystem()->getBindings(InputType::PLAYER2);

	switch (binding)
	{
	case KeyBindingsSelector::P1_UP:
	{
		m_p1_map.up = key;
		break;
	}
	case KeyBindingsSelector::P1_BACK:
	{
		m_p1_map.back = key;
		break;
	}
	case KeyBindingsSelector::P1_DOWN:
	{
		m_p1_map.down = key;
		break;
	}
	case KeyBindingsSelector::P1_FIRE:
	{
		m_p1_map.fire = key;
		break;
	}
	case KeyBindingsSelector::P1_LEFT:
	{
		m_p1_map.left = key;
		break;
	}
	case KeyBindingsSelector::P1_POWERUP:
	{
		m_p1_map.power_up = key;
		break;
	}
	case KeyBindingsSelector::P1_RIGHT:
	{
		m_p1_map.right = key;
		break;
	}
	case KeyBindingsSelector::P1_SELECT:
	{
		m_p1_map.select = key;
		break;
	}
	case KeyBindingsSelector::P2_BACK:
	{
		m_p2_map.back = key;
		break;
	}
	case KeyBindingsSelector::P2_DOWN:
	{
		m_p2_map.down = key;
		break;
	}
	case KeyBindingsSelector::P2_FIRE:
	{
		m_p2_map.fire = key;
		break;
	}
	case KeyBindingsSelector::P2_LEFT:
	{
		m_p2_map.left = key;
		break;
	}
	case KeyBindingsSelector::P2_POWERUP:
	{
		m_p2_map.power_up = key;
		break;
	}
	case KeyBindingsSelector::P2_RIGHT:
	{
		m_p2_map.right = key;
		break;
	}
	case KeyBindingsSelector::P2_SELECT:
	{
		m_p2_map.select = key;
		break;
	}
	case KeyBindingsSelector::P2_UP:
	{
		m_p2_map.up = key;
		break;
	}
	}
	m_game->getInputSystem()->updateBindings(m_p1_map, m_p2_map);
}

std::string InControls::findKeyInKeys(DirectX::Keyboard::Keys key)
{
	for (auto& k : m_keys)
	{
		if (k.first == key)
		{
			return k.second;
		}
	}
	return "NO KEY";
}

void InControls::playMenuSound()
{
	const auto i = AudioManager::loadSoundEffect("Assets/ui_browse.wav");
	auto* inst = AudioManager::createEffectInstance(i);
	AudioManager::playSound(inst, 0.5F);
}

void InControls::end()
{
	for (auto& entity : m_entities)
	{
		m_game->ecs()->destroyEntity(entity.first);
	}
}

bool InControls::loadSprite(const std::string& texture_path, ECS::Entity entity, const std::pair<float, float>& transform,
							const std::pair<float, float>& relative_pos, const std::pair<float, float>& scale, 
							const DirectX::SimpleMath::Color& color, float z_order)
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

InControls::KeyBindingsSelector InControls::changeSelected(KeyBindingsSelector current_selection, int i)
{
	auto item = static_cast<int>(current_selection) + i;
	if (item >= 17)
	{
		item = 0;
	}
	else if (item < 0)
	{
		item = 15;
	}
	return static_cast<KeyBindingsSelector>(item);
}
