// Components
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/TextRenderComponent.h>
#include <Components/HUDImageComponent.h>
#include <Components/HUDTextComponent.h>
#include <Data/TagData.h>

// Systems
#include <Systems/CollisionSystem.h>
#include <Systems/BoundarySystem.h>
#include <Controllers/PlayerController.h>
#include <Systems/BulletSpawnerSystem.h>
#include <Systems/TargetingSystem.h>

// Game
#include "ECS.h"
#include <Game/Game.h>
#include "ExampleState.h"
#include <Game/EntityFactory.h>

void ExampleState::start()
{
	auto font_id   = m_game->getGameRenderer()->loadFont(m_game->getDevice(), "Assets/arial.spritefont");
	auto grass_id  = m_game->getGameRenderer()->loadTexture(m_game->getDevice(), "Assets/grass.dds");
	auto player_id = m_game->getGameRenderer()->loadTexture(m_game->getDevice(), "Assets/test_player.dds");

	auto player1 = EntityFactory::spawnPlayer(*m_game->ecs(), *m_game->getGameRenderer(), player_id, {1.F, 1.F},
	                                          *m_game->getInputSystem(), InputType::PLAYER1);
	//auto player2 = EntityFactory::spawnPlayer(*m_game->ecs(), *m_game->getGameRenderer(), player_id, *m_game->getInputSystem(), InputType::PLAYER2);
	m_player_entity_list.push_back(player1);
	//m_player_list.push_back(player2);

	//EntityFactory::spawnEnemy(*m_game->ecs(), *m_game->getGameRenderer(), player_id);

	// also grass image
	auto entity1 = m_game->ecs()->createEntity();
	TransformComponent transform;
	transform.m_position = {960.f, 540.f};
	transform.m_scale    = {0.3f, 0.3f};
	m_game->ecs()->addComponent(entity1, transform);
	SpriteComponent sprite;
	m_game->getGameRenderer()->setupSpriteComponent(grass_id, sprite);
	sprite.m_relative_position = {-500.f, 0.f};
	sprite.m_z_order           = 0.0f;
	m_game->ecs()->addComponent(entity1, sprite);
	TextRenderComponent text;
	text.m_font_id           = font_id;
	text.m_relative_scale    = {2.5f, 2.5f};
	text.m_relative_position = {500.f, 0.f};
	m_game->ecs()->addComponent(entity1, text);

	{
		// grass image
		auto entity = m_game->ecs()->createEntity();
		TransformComponent hud_transform1;
		hud_transform1.m_position = {960.f, 150.f};
		m_game->ecs()->addComponent(entity, hud_transform1);
		HUDTextComponent hud_text1;
		hud_text1.m_font_id = font_id;
		hud_text1.m_text    = "Preview HUD text.";
		m_game->ecs()->addComponent(entity, hud_text1);
		HUDImageComponent hud_image1;
		m_game->getGameRenderer()->setupHUDImageComponent(grass_id, hud_image1);
		hud_image1.m_relative_position = {-350.f, 0.f};
		hud_image1.m_relative_scale    = {0.1f, 0.1f};
		hud_image1.m_colour            = DirectX::Colors::Red;
		m_game->ecs()->addComponent(entity, hud_image1);
	}
}

void ExampleState::updateObjects(GameData* gd)
{

}

void ExampleState::end() {}
