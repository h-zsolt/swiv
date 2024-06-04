#include "InGame.h"

// Components
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/TextRenderComponent.h>
#include <Components/HUDImageComponent.h>
#include <Components/HUDTextComponent.h>
#include <Data/TagData.h>
#include <Data/EnemySpawnerData.h>
#include <Components/SpriteAnimComponent.h>

// Systems
#include <Systems/CollisionSystem.h>
#include <Systems/BoundarySystem.h>
#include <Controllers/PlayerController.h>
#include <Systems/BulletSpawnerSystem.h>
#include <Game/EnemySpawnerSystem.h>
#include <Game/PowerUpSystem.h>
#include <Systems/NoEnemiesWinSystem.h>
#include <Systems/TargetingSystem.h>
#include <Audio/AudioManager.hpp>
#include <Systems/EnvironmentSpawnSystem.h>
#include <Systems/MovementSystem.h>
#include <Systems/ColliderMovementSystem.h>
#include <Systems/RotationSystem.h>

// Game
#include <GameEvent.h>

#include "ECS.h"
#include <Game/Game.h>
#include "PauseMenu.h"
#include <Game/EntityFactory.h>

auto InGame::getPlayerFromEntity(ECS::Entity entity) noexcept
{
	return std::find_if(m_player_list.begin(), m_player_list.end(), [entity](const SWIVPlayer& player) {return player.entity() == entity; });
}

void InGame::playExplosion(const DirectX::SimpleMath::Vector2& position) noexcept
{
	auto* ecs = m_game->ecs();
	exp = m_game->ecs()->createEntity();

	TransformComponent transform{};
	transform.m_position = position;
	transform.m_scale = EXPLOSION_ENTITY_SCALE;
	ecs->addComponent(exp, transform);

	SpriteAnimComponent anim;
	anim.m_texture_id = static_cast<int>(m_explosion_texture_atlas_id);
	anim.m_playback_rate_seconds = EXPLOSION_PLAYBACK_RATE;

	RECT frame{};

	frame.top = 192;
	frame.left = 192;
	frame.bottom = 256;
	frame.right = 256;
	anim.m_frames.push_back(frame);

	frame.top = 192;
	frame.left = 128;
	frame.bottom = 256;
	frame.right = 192;
	anim.m_frames.push_back(frame);

	frame.top = 192;
	frame.left = 64;
	frame.bottom = 256;
	frame.right = 128;
	anim.m_frames.push_back(frame);

	frame.top = 192;
	frame.left = 0;
	frame.bottom = 256;
	frame.right = 64;
	anim.m_frames.push_back(frame);

	frame.top = 128;
	frame.left = 192;
	frame.bottom = 192;
	frame.right = 256;
	anim.m_frames.push_back(frame);

	frame.top = 128;
	frame.left = 128;
	frame.bottom = 192;
	frame.right = 192;
	anim.m_frames.push_back(frame);

	frame.top = 128;
	frame.left = 64;
	frame.bottom = 192;
	frame.right = 128;
	anim.m_frames.push_back(frame);

	frame.top = 128;
	frame.left = 0;
	frame.bottom = 192;
	frame.right = 64;
	anim.m_frames.push_back(frame);

	frame.top = 64;
	frame.left = 192;
	frame.bottom = 128;
	frame.right = 256;
	anim.m_frames.push_back(frame);

	frame.top = 64;
	frame.left = 128;
	frame.bottom = 128;
	frame.right = 192;
	anim.m_frames.push_back(frame);

	frame.top = 64;
	frame.left = 64;
	frame.bottom = 128;
	frame.right = 128;
	anim.m_frames.push_back(frame);

	frame.top = 64;
	frame.left = 0;
	frame.bottom = 128;
	frame.right = 64;
	anim.m_frames.push_back(frame);

	frame.top = 0;
	frame.left = 192;
	frame.bottom = 64;
	frame.right = 256;
	anim.m_frames.push_back(frame);

	frame.top = 0;
	frame.left = 128;
	frame.bottom = 64;
	frame.right = 192;
	anim.m_frames.push_back(frame);

	frame.top = 0;
	frame.left = 64;
	frame.bottom = 64;
	frame.right = 128;
	anim.m_frames.push_back(frame);

	frame.top = 0;
	frame.left = 0;
	frame.bottom = 64;
	frame.right = 64;
	anim.m_frames.push_back(frame);

	frame.top = 0;
	frame.left = 0;
	frame.bottom = 0;
	frame.right = 0;
	anim.m_frames.push_back(frame);

	anim.m_origin = 0.5f * DirectX::SimpleMath::Vector2(static_cast<float>(anim.m_frames[0].right - anim.m_frames[0].left), static_cast<float>(anim.m_frames[0].bottom - anim.m_frames[0].top));

	ecs->addComponent(exp, anim);

	m_explosion_entity_list.push_back(exp);
}

void InGame::start()
{
	// Setup events.
	game_event_callback = m_game->getEventBus()->subscribe(this, &InGame::onGameEvent);

	auto* p_ecs = m_game->ecs();

	// Stop any previous background music that's playing
	AudioManager::stopAll();

	// Load level.
	m_game->importLevel("Assets/level" + std::to_string(m_game->getSelectedLevel()) + ".LEVEL", m_level_entity_list);
	if (m_game->getSelectedLevel() == 4) //InMenu::Levels::LEVEL4 can't be seen because it's private, and I'm scared to make it public
	{
		m_current_camera_scroll_speed = 0;
		m_check_enemy_system = true;
	}
	else
	{
		m_check_enemy_system = false;
	}

	// spawn the environment after the level has loaded
	m_game->getEnvironmentSpawner()->spawnEnvironment(*m_game->ecs(), *m_game->getGameRenderer());

	// Setup players.
	createPlayers();

	// Setup game hud.
	createHUD(p_ecs);

	// Set game start values.
	setHUDText(m_p1_score_hud_entity, m_player_list[0].getScoreHUDText());
	setHUDText(m_p1_lives_hud_entity, m_player_list[0].getLivesHUDText());

	// If there are two players selected from the menu
	if (m_game->getNumSelectedPlayers() > 0)
	{
		setHUDText(m_p2_score_hud_entity, m_player_list[1].getScoreHUDText());
		setHUDText(m_p2_lives_hud_entity, m_player_list[1].getLivesHUDText());
	}

	// Start background music
	const auto i = AudioManager::loadSoundEffect("Assets/level1.wav");
	auto* inst = AudioManager::createEffectInstance(i);
	AudioManager::playSound(inst, true, 0.3F);

	// Setup explosions
	m_explosion_texture_atlas_id = m_game->getGameRenderer()->loadTexture(m_game->getDevice(), "Assets/explosion.dds");
	m_explosion_entity_list.reserve(EXPLOSION_RESERVE_NUMBER);
}

void InGame::updateObjects(GameData* gd)
{
	if (!m_reached_end)
	{

		auto* input_system = m_game->getInputSystem();
		input_system->applyPlayerInput(*m_game->ecs());
		if (input_system->keyboardStateTracker().IsKeyPressed(Keyboard::Escape) && !m_reached_end)
		{
			m_game->pauseState(std::make_unique<PauseMenu>());
		}

		if (m_game->ecs()->entityValid(m_player_entity_list[0]))
		{
			auto& key_data = m_game->ecs()->getComponent<InputData>(m_player_entity_list[0]);
			key_data = input_system->getBindings(InputType::PLAYER1);
		}

		if (m_game->getNumSelectedPlayers() > 0)
		{
			if (m_game->ecs()->entityValid(m_player_entity_list[1]))
			{
				auto& key_data = m_game->ecs()->getComponent<InputData>(m_player_entity_list[1]);
				key_data = input_system->getBindings(InputType::PLAYER2);
			}
		}

		// Timers.
		m_elapsed_time += gd->m_dt;

		// For each player in the player list
		for (auto& player : m_player_list)
		{
			// Update player inputs
			auto& key_data = m_game->ecs()->getComponent<InputData>(player.entity());
			if (player.playerNumber() == P1_PLAYER_NUMBER)
			{
				key_data = input_system->getBindings(InputType::PLAYER1);
			}
			else if (player.playerNumber() == P2_PLAYER_NUMBER)
			{
				key_data = input_system->getBindings(InputType::PLAYER2);
			}

			// Check if the player is not vulnerable
			if (!player.vulnerable())
			{
				// Check if the invulnerable period has elapsed for the player
				if ((m_elapsed_time - player.lastHitTime()) >= PLAYER_INVULNERABLE_DURATION)
				{
					// set player collider collidable
					auto& cc = m_game->ecs()->getComponent<Box2DColliderData>(player.entity());
					if (!cc.m_collidable) cc.m_collidable = true;

					// Make the player vulnerable
					player.makeVulnerable(*m_game->ecs());
				}
			}
		}

		// Update systems.
		m_game->getPlayerController()->update(*m_game->ecs(), gd->m_dt);
		m_game->getTargetingSystem()->update(*m_game->ecs(), m_player_entity_list);
		m_game->getBulletSpawner()->update(*m_game->ecs(), *m_game->getGameRenderer(), gd->m_dt);
		m_game->getEnemySpawner()->update(*m_game->ecs(), *m_game->getGameRenderer(), gd->m_dt);
		m_game->getCollisionSystem()->update(*m_game->ecs(), gd->m_dt);
		m_game->getPowerUpSystem()->update(*m_game->ecs());

		auto window_dims = m_game->getWindowDimensions();
		m_game->getBoundarySystem()->update(*m_game->ecs(), DirectX::SimpleMath::Vector2(window_dims.x < M_BOUNDARY_WIDTH ? window_dims.x : M_BOUNDARY_WIDTH, window_dims.y), gd->m_dt, m_game->getGameRenderer()->getGameCameraPosition().y);
		m_game->getMovementSystem()->update(*m_game->ecs(), gd->m_dt);
		m_game->getRotationSystem()->update(*m_game->ecs(), gd->m_dt);
		m_game->getColliderMovementSystem()->update(*m_game->ecs(), gd->m_dt);

		if (m_check_enemy_system)
		{
			m_game->getNoEnemiesWinSystem()->update(*m_game->ecs());
		}

		if (m_game->getGameRenderer()->getGameCameraPosition().y >= LEVEL_END_CAMERA_TARGET)
		{
			onLevelEnd();
		}

		// Auto scroll camera forward.
		m_game->getGameRenderer()->addGameCameraOffset({ 0.f, m_current_camera_scroll_speed * gd->m_dt, 0.f });
	}
	else
	{
		// Return to menu on key press.
		auto* input_system = m_game->getInputSystem();
		if (input_system->keyboardStateTracker().IsKeyPressed(input_system->getBindings(InputType::PLAYER1).fire))
		{
			m_game->setState(std::make_unique<InMenu>());
		}
	}
}

void InGame::end()
{
	m_player_list.clear();

	// Unsubscribe EventBus callbacks
	m_game->getEventBus()->unsubscribe(game_event_callback);

	AudioManager::stopAll();
	// Play start menu background music
	const auto i = AudioManager::loadSoundEffect("Assets/start_menu.wav");
	auto* inst = AudioManager::createEffectInstance(i);
	AudioManager::playSound(inst, true, 0.3F);
}

void InGame::onGameEvent(GameEvent* game_event)
{

	constexpr GameEventType EnemyShotEvent = GameEventType::GameEventType_1;
	constexpr GameEventType PlayerHitEvent = GameEventType::GameEventType_2;
	constexpr GameEventType EnemyDestroyedEvent = GameEventType::GameEventType_3;
	constexpr GameEventType VictoryConditionMet = GameEventType::GameEventType_4;

	switch (game_event->m_event_type)
	{
	case EnemyShotEvent: onEnemyShot(game_event->m_player_id); break;
	case PlayerHitEvent: onPlayerHit(game_event->m_player_id); break;
	case EnemyDestroyedEvent: onEnemyDestroyed(game_event->m_player_id, game_event->m_enemy_id); break;
	case VictoryConditionMet: onLevelEnd(); break;
	default: std::cout << "unhandled GameEvent in InGame State\n"; break;
	}
}

void InGame::onPlayerHit(ECS::Entity player)
{
	// Get the player that was hit
	auto hit_player = getPlayerFromEntity(player);

	// Safety check that a player was found
	if (hit_player != m_player_list.end())
	{
		// Check the hit player is vulnerable
		if (hit_player->vulnerable())
		{
			// Check the player has no lives remaining
			if (hit_player->lives() == NO_LIVES_REMAINING_AMOUNT)
			{
				onPlayerDestroyed(hit_player);
				return;
			}

			// Update the player's lives total
			hit_player->removeSingleLife();

			// set player to be uncollidable
			auto& cc = m_game->ecs()->getComponent<Box2DColliderData>(hit_player->entity());
			if (cc.m_collidable) cc.m_collidable = false;

			// Make the player invulnerable
			hit_player->makeInvulnerable(*m_game->ecs(), m_elapsed_time);

			// Check which player has been hit
			if (hit_player->playerNumber() == P1_PLAYER_NUMBER)
			{
				// Update player 1's lives HUD display
				setHUDText(m_p1_lives_hud_entity, hit_player->getLivesHUDText());
			}
			else
			{
				// Update player 2's lives HUD display
				setHUDText(m_p2_lives_hud_entity, hit_player->getLivesHUDText());
			}
		}
	}
}

void InGame::onPlayerDestroyed(CPlayerIter player) noexcept
{
	auto* ecs = m_game->ecs();

	// Play explosion effect at player location
	const auto& player_transform = ecs->getComponent<TransformComponent>(player->entity());
	playExplosion(player_transform.m_position);

	// Destroy the player's entity
	ecs->destroyEntity(player->entity());

	// Remove the player from the player list
	 m_player_list.erase(player);

	// Check if all players have been destroyed
	if (m_player_list.size() == 0)
	{
		// Game over
		onGameOver();
	}
}

void InGame::onEnemyShot(ECS::Entity player)
{
	// Get the player object from the player entity
	auto swiv_player = getPlayerFromEntity(player);

	// Safety check the player is valid
	if (swiv_player != m_player_list.end())
	{
		// Add score value to the player's score
		swiv_player->addScore(ENEMY_SHOT_SCORE_VALUE);

		// Check if the player is player 1
		if (swiv_player->playerNumber() == P1_PLAYER_NUMBER)
		{
			// Update player 1's score display
			setHUDText(m_p1_score_hud_entity, swiv_player->getScoreHUDText());
		}
		// Else check if the player is player 2
		else if (swiv_player->playerNumber() == P2_PLAYER_NUMBER)
		{
			// Update player 2's score display
			setHUDText(m_p2_score_hud_entity, swiv_player->getScoreHUDText());
		}
	}
}

void InGame::onEnemyDestroyed(ECS::Entity player, ECS::Entity enemy) noexcept
{
	// Get the player object from the player entity
	auto swiv_player = getPlayerFromEntity(player);

	// Safety check the player is valid
	if (swiv_player != m_player_list.end())
	{
		// Add score value to the player's score
		swiv_player->addScore(ENEMY_DESTROYED_SCORE_VALUE);

		// Check if the player is player 1
		if (swiv_player->playerNumber() == P1_PLAYER_NUMBER)
		{
			// Update player 1's score display
			setHUDText(m_p1_score_hud_entity, swiv_player->getScoreHUDText());
		}
		// Else check if the player is player 2
		else if (swiv_player->playerNumber() == P2_PLAYER_NUMBER)
		{
			// Update player 2's score display
			setHUDText(m_p2_score_hud_entity, swiv_player->getScoreHUDText());
		}
	}

	// Play explosion effect at enemy location
	const auto& enemy_transform = m_game->ecs()->getComponent<TransformComponent>(enemy);
	playExplosion(enemy_transform.m_position);
}

void InGame::createPlayers()
{
	auto* p_ecs			= m_game->ecs();
	auto* game_renderer = m_game->getGameRenderer();
	auto* input_system  = m_game->getInputSystem();

	auto player_tex_id = game_renderer->loadTexture(m_game->getDevice(), "Assets/playerShip.dds");

	auto player1 = EntityFactory::spawnPlayer(*m_game->ecs(), *game_renderer, player_tex_id, { 1.F,1.F }, *input_system, InputType::PLAYER1);
	m_player_entity_list.push_back(player1);
	m_player_list.emplace_back(player1, P1_PLAYER_NUMBER);

	// Only setup player two if two players was selected from the menu.
	if (m_game->getNumSelectedPlayers() > 0)
	{
		auto player2_tex_id = game_renderer->loadTexture(m_game->getDevice(), "Assets/tankRed.dds");
		auto player2 = EntityFactory::spawnPlayer(*m_game->ecs(), *game_renderer, player2_tex_id, { 0.75F,0.75F }, *input_system, InputType::PLAYER2);
		m_player_entity_list.push_back(player2);
		m_player_list.emplace_back(player2, P2_PLAYER_NUMBER);
	}

	m_num_starting_players = m_player_list.size();
}


void InGame::createHUD(ECS::ECS* p_ecs)
{
	auto text_font_id = m_game->getGameRenderer()->loadFont(m_game->getDevice(), "Assets/arial.spritefont");

	// Create HUD entities for player 1
	m_p1_score_hud_entity = m_hud_entity_list.emplace_back(std::move(createHUDTextEntity(p_ecs, text_font_id, P1_SCORE_TEXT_POSITION, P1_SCORE_TEXT_REL_SCALE)));
	m_p1_lives_hud_entity = m_hud_entity_list.emplace_back(std::move(createHUDTextEntity(p_ecs, text_font_id, P1_LIVES_TEXT_POSITION, P1_LIVES_TEXT_REL_SCALE)));

	// Create HUD entities for player 2 if two players was selected from the menu
	if (m_game->getNumSelectedPlayers() > 0)
	{
		m_p2_score_hud_entity = m_hud_entity_list.emplace_back(std::move(createHUDTextEntity(p_ecs, text_font_id, P2_SCORE_TEXT_POSITION, P2_SCORE_TEXT_REL_SCALE)));
		m_p2_lives_hud_entity = m_hud_entity_list.emplace_back(std::move(createHUDTextEntity(p_ecs, text_font_id, P2_LIVES_TEXT_POSITION, P2_LIVES_TEXT_REL_SCALE)));
	}

	// Create game HUD entities
	m_level_complete_message_hud_entity = 
		m_hud_entity_list.emplace_back(std::move(
			createHUDTextEntity(p_ecs, text_font_id, COMPLETE_MESSAGE_TEXT_POSITION, COMPLETE_MESSAGE_TEXT_REL_SCALE, "Level Complete", false)));
	m_game_over_message_hud_entity =
		m_hud_entity_list.emplace_back(std::move(
			createHUDTextEntity(p_ecs, text_font_id, GAME_OVER_MESSAGE_TEXT_POSITION, M_GAME_OVER_MESSAGE_TEXT_REL_SCALE, "Game Over", false)));
}

void InGame::setHUDText(ECS::Entity hud_entity, const std::string& text) noexcept
{
	// Set the hud entity's hud text component's text attribute
	m_game->ecs()->getComponent<HUDTextComponent>(hud_entity).m_text = text;
}

ECS::Entity InGame::createHUDTextEntity(ECS::ECS* p_ecs,
	size_t font_id,
	const DirectX::SimpleMath::Vector2& position,
	const DirectX::SimpleMath::Vector2& relative_scale,
	const std::string& default_text,
	bool visible)
{
	auto& entity = m_hud_entity_list.emplace_back(p_ecs->createEntity());
	TransformComponent transform;
	transform.m_position = position;
	p_ecs->addComponent(entity, transform);
	HUDTextComponent text;
	text.m_font_id = font_id;
	text.m_text = default_text;
	text.m_relative_scale = relative_scale;
	text.m_render = visible;
	p_ecs->addComponent(entity, text);
	return entity;
}

void InGame::onLevelEnd()
{
	auto* p_ecs = m_game->ecs();

	m_reached_end = true;

	hideGameHUD(p_ecs);
	// Show level complete HUD.
	auto& lc_message_text = p_ecs->getComponent<HUDTextComponent>(m_level_complete_message_hud_entity);
	lc_message_text.m_render = true;

	if (m_num_starting_players > 1)
	{
		lc_message_text.m_text = "Level Complete\nP1 Final Score: " + std::to_string(m_player_list[0].score()) + "\nP2 Final Score: " + std::to_string(m_player_list[1].score()) +
			"\nP1 press FIRE to return to menu";

	}
	else
	{
		lc_message_text.m_text = "Level Complete\nFinal Score: " + std::to_string(m_player_list[0].score()) + "\nP1 press FIRE to return to menu";
	}
}

void InGame::onGameOver()
{
	auto* p_ecs = m_game->ecs();

	m_reached_end = true;
	m_current_camera_scroll_speed = 0.F;

	hideGameHUD(p_ecs);
	// Show game over HUD.
	auto& go_message_text = p_ecs->getComponent<HUDTextComponent>(m_game_over_message_hud_entity);
	go_message_text.m_render = true;
	go_message_text.m_text = "Game Over\nFinal Score: " + std::to_string(m_player_score) + "\nP1 press FIRE to return to menu";
}


void InGame::hideGameHUD(ECS::ECS* p_ecs)
{
	// Hide game HUD.
	p_ecs->getComponent<HUDTextComponent>(m_p1_score_hud_entity).m_render = false;
	p_ecs->getComponent<HUDTextComponent>(m_p1_lives_hud_entity).m_render = false;

	if (m_num_starting_players > 1)
	{
		p_ecs->getComponent<HUDTextComponent>(m_p2_score_hud_entity).m_render = false;
		p_ecs->getComponent<HUDTextComponent>(m_p2_lives_hud_entity).m_render = false;
	}
}
