#include <pch.h>
// Components
#include "Tags.h"
#include <Components/SpriteComponent.h>
#include <Components/SpriteAnimComponent.h>
#include <Components/TransformComponent.h>
#include <Components/TextRenderComponent.h>
#include <Components/HUDTextComponent.h>
#include <Components/HUDImageComponent.h>
#include <Data/InputData.h>
#include <Data/MoveData.h>
#include <Data/ButtonData.h>
#include <Data/RotationData.h>
#include <Data/Box2DColliderData.h>
#include <Data/BulletData.h>
#include <Data/LifetimeData.h>
#include <Data/TargetData.h>
#include <Data/TagData.h>
#include <Data/HealthData.h>
#include <Data/EnemySpawnerData.h>
#include <Components/BulletParentComponent.h>
#include <Components/BoundComponent.h>
#include <Components/HealthBarComponent.h>
#include <Components/BulletDamageComponent.h>
#include <PowerUpData.h>
#include <Components/StagedBehaviourComponent.h>
#include <Components/FiringModesComponent.h>
#include <Components/FixedShootingPointComponent.h>
#include <Components/ADSpawnerComponent.h>

// Systems
#include "GameRenderer.h"
#include <Systems/InputSystem.h>
#include <Systems/MovementSystem.h>
#include <Systems/RotationSystem.h>
#include <Systems/BoundarySystem.h>
#include <Systems/CollisionSystem.h>
#include <Systems/CollisionResolutionSystem.h>
#include <Systems/BulletSpawnerSystem.h>
#include "EnemySpawnerSystem.h"
#include "PowerUpSystem.h"
#include <Controllers/PlayerController.h>
#include <Systems/LifetimeSystem.h>
#include <Systems/TargetingSystem.h>
#include <Systems/MoveTowardSystem.h>
#include <Systems/MoveForwardSystem.h>
#include <Systems/RotateTowardSystem.h>
#include <Systems/ColliderMovementSystem.h>
#include <Audio/AudioManager.hpp>
#include <Systems/HealthBarSystem.h>
#include <Systems/HealthSystem.h>
#include <Systems/EnvironmentSpawnSystem.h>
#include <Systems/StagedBehaviourSystem.h>
#include <Systems/FireControlSystem.h>
#include <Systems/FiringModeSystem.h>
#include <Systems/FixedShootingPointSystem.h>
#include <Systems/ADSpawnerSystem.h>
#include <Systems/NoEnemiesWinSystem.h>

// Core
#include "Game.h"
#include <Game/States/InGame.h>
#include <Game/States/ExampleState.h>

#include "ThirdParty/yaml-cpp/yaml.h"
#include <Data/EnvironmentTag.h>

// Serialization
template <>
struct YAML::convert<DirectX::SimpleMath::Vector2>
{
	static Node encode(const DirectX::SimpleMath::Vector2& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node& node, DirectX::SimpleMath::Vector2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
		{
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

template <>
struct YAML::convert<DirectX::SimpleMath::Color>
{
	static Node encode(const DirectX::SimpleMath::Color& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node& node, DirectX::SimpleMath::Color& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
		{
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};

Game::Game() noexcept : m_input_system(nullptr), m_movement_system(nullptr),
m_collision_system(nullptr), m_c_resolution_system(nullptr)
{
}

void Game::start()
{
	// Create event bus
	m_event_bus = std::make_unique<EventBus>();

	// Register tags
	ecs()->registerComponent<PlayerTag>();
	ecs()->registerComponent<EnemyTag>();
	ecs()->registerComponent<InRangeTriggerTag>();
	ecs()->registerComponent<MoveTowardTag>();
	ecs()->registerComponent<MoveForwardTag>();
	ecs()->registerComponent<RotateTowardTag>();
	ecs()->registerComponent<EnvironmentTag>();
	ecs()->registerComponent<HoldFireTag>();
	ecs()->registerComponent<StartFireTag>();
	ecs()->registerComponent<ChangeFireModeTag>();
	ecs()->registerComponent<StartFixedPointShootingTag>();
	ecs()->registerComponent<StopFixedPointShootingTag>();
	ecs()->registerComponent<SpawnADsTag>();
	ecs()->registerComponent<WinConditionTag>(); //this is a workaround

	// Register components.
	ecs()->registerComponent<TransformComponent>();
	ecs()->registerComponent<SpriteComponent>();
	ecs()->registerComponent<SpriteAnimComponent>();
	ecs()->registerComponent<TextRenderComponent>();
	ecs()->registerComponent<InputData>();
	ecs()->registerComponent<MoveData>();
	ecs()->registerComponent<ButtonData>();
	ecs()->registerComponent<BulletData>();
	ecs()->registerComponent<EnemySpawnerData>();
	ecs()->registerComponent<RotationData>();
	ecs()->registerComponent<HUDTextComponent>();
	ecs()->registerComponent<HUDImageComponent>();
	ecs()->registerComponent<Box2DColliderData>();
	ecs()->registerComponent<LifetimeData>();
	ecs()->registerComponent<TargetData>();
	ecs()->registerComponent<TagData>();
	ecs()->registerComponent<BulletParentComponent>();
	ecs()->registerComponent<HealthBarComponent>();
	ecs()->registerComponent<BoundComponent>();
	ecs()->registerComponent<HealthData>();
	ecs()->registerComponent<BulletDamageComponent>();
	ecs()->registerComponent<PowerUpData>();
	ecs()->registerComponent<StagedBehaviourComponent>();
	ecs()->registerComponent<FiringModesComponent>();
	ecs()->registerComponent<FixedShootingPointComponent>();
	ecs()->registerComponent<ADSpawnerComponent>();

	// Register systems.
	m_game_renderer					 = std::make_unique<GameRenderer>();
	m_input_system					 = ecs()->registerSystem<InputSystem>();
	m_movement_system				 = ecs()->registerSystem<MovementSystem>();
	m_rotation_system				 = ecs()->registerSystem<RotationSystem>();
	m_in_range_trigger_system		 = ecs()->registerSystem<InRangeTriggerSystem>();
	m_move_forward_system			 = ecs()->registerSystem<MoveForwardSystem>();
	m_move_toward_system			 = ecs()->registerSystem<MoveTowardSystem>();
	m_rotate_toward_system			 = ecs()->registerSystem<RotateTowardSystem>();
	m_player_controller				 = ecs()->registerSystem<PlayerController>();
	m_bullet_spawner				 = ecs()->registerSystem<BulletSpawnerSystem>();
	m_targeting_system				 = ecs()->registerSystem<TargetingSystem>();
	m_lifetime_system				 = ecs()->registerSystem<LifetimeSystem>();
	m_boundary_system				 = ecs()->registerSystem<BoundarySystem>();
	m_collision_system				 = ecs()->registerSystem<CollisionSystem>();
	m_c_resolution_system			 = ecs()->registerSystem<CollisionResolutionSystem>();
	m_collider_movement_system		 = ecs()->registerSystem<ColliderMovementSystem>();
	m_environment_spawn_system		 = ecs()->registerSystem<EnvironmentSpawnSystem>();
	m_healthbar_system				 = ecs()->registerSystem<HealthBarSystem>();
	m_health_system					 = ecs()->registerSystem<HealthSystem>();
	m_enemy_spawner					 = ecs()->registerSystem<EnemySpawnerSystem>();
	m_power_up_system                = ecs()->registerSystem<PowerUpSystem>();
	m_staged_behaviour_system		 = ecs()->registerSystem<StagedBehaviourSystem>();
	m_fire_control_system = ecs()->registerSystem<FireControlSystem>();
	m_firing_mode_system = ecs()->registerSystem<FiringModeSystem>();
	m_fixed_shooting_point_system = ecs()->registerSystem<FixedShootingPointSystem>();
	m_ad_spawner_system = ecs()->registerSystem<ADSpawnerSystem>();
	m_no_enemies_win_system = ecs()->registerSystem<NoEnemiesWinSystem>();

	// Init Systems
	m_game_renderer->init(*ecs(), getDevice(), getDeviceContext());
	m_input_system->init(*ecs(), getWindow());
	m_in_range_trigger_system->init(*ecs());
	m_move_forward_system->init(*ecs());
	m_move_toward_system->init(*ecs());
	m_rotate_toward_system->init(*ecs());
	m_movement_system->init(*ecs());
	m_rotation_system->init(*ecs());
	m_player_controller->init(*ecs());
	m_targeting_system->init(*ecs());
	m_lifetime_system->init(*ecs());
	m_boundary_system->init(*ecs(), m_event_bus.get());
	m_healthbar_system->init(*ecs(), m_game_renderer.get());
	m_health_system->init(*ecs());
	m_collision_system->init(*ecs(), m_event_bus.get());
	m_c_resolution_system->init(*ecs(), m_event_bus.get());
	m_collider_movement_system->init(*ecs());
	m_environment_spawn_system->init(*ecs(), *m_game_renderer, getDevice());
	m_staged_behaviour_system->init(*ecs());
	m_fire_control_system->init(*ecs());
	m_firing_mode_system->init(*ecs());
	m_fixed_shooting_point_system->init(*ecs());
	m_ad_spawner_system->init(*ecs());
	m_no_enemies_win_system->init(*ecs(), m_event_bus.get());

	// Init AudioManager
	AudioManager::init();

	// needs to be after the renderer is init()
	auto bullet_id = getGameRenderer()->loadTexture(getDevice(), "Assets/laserRed02.dds");
	m_bullet_spawner->init(*ecs(), bullet_id);
	m_enemy_spawner->init(*ecs(), *getGameRenderer(), getDevice());
	m_power_up_system->init(*ecs(), *getGameRenderer(), getDevice(), *getEventBus());

	auto dot_id = getGameRenderer()->loadTexture(getDevice(), "Assets/dot4x4.dds");
	m_healthbar_system->setTextureIDs(dot_id, dot_id, dot_id);

	ShowCursor(false);

	// Set the State
	setState(std::make_unique<InMenu>());

	// Play start menu background music
	const auto i = AudioManager::loadSoundEffect("Assets/start_menu.wav");
	auto* inst = AudioManager::createEffectInstance(i);
	AudioManager::playSound(inst, true, 0.3F);
}

void Game::updateObjects(GameData* gd)
{
	auto& r_ecs = *ecs();

	// KeyBoard Input
	m_input_system->readInput(r_ecs);

	// Renderer
	m_game_renderer->tick(gd->m_dt, r_ecs);

	if (!m_paused_game)
	{
		// Opti Group
		m_lifetime_system->update(r_ecs, gd->m_dt);

		// Input Group
		m_in_range_trigger_system->update(r_ecs);
		m_move_forward_system->update(r_ecs);
		m_move_toward_system->update(r_ecs);
		m_rotate_toward_system->update(r_ecs);


		// Resolution Group
		m_c_resolution_system->update();
		m_health_system->update(*ecs());
		m_healthbar_system->update(*ecs(), gd->m_dt, m_game_renderer->getGameCameraPosition().y);
		m_staged_behaviour_system->update(*ecs(), gd->m_dt);
		m_fixed_shooting_point_system->update(*ecs());
		m_fire_control_system->update(*ecs());
		m_firing_mode_system->update(*ecs());
		m_ad_spawner_system->update(*ecs());
	}
	// Update State Logic
	m_state->updateObjects(gd);
}

void Game::renderObjects()
{
	m_game_renderer->render(*ecs(), getDeviceContext());
}

void Game::onActivated()
{
	ScarleGame::onActivated();
	//To be implemented
}

void Game::onDeactivated()
{
	ScarleGame::onDeactivated();
	//To be implemented
}

void Game::onSuspending()
{
	ScarleGame::onSuspending();
	//To be implemented
}

void Game::onResuming()
{
	ScarleGame::onResuming();
	//To be implemented
}

void Game::onWindowSizeChanged(int _width, int _height)
{
	ScarleGame::onWindowSizeChanged(_width, _height);
	//To be implemented
}

void Game::setState(std::unique_ptr<State> state)
{
	m_game_renderer->setGameCameraPosition(M_DEFAULT_GAME_CAMERA_POSITION);

	if (m_state != nullptr)
	{
		m_state->end();
	}

	ecs()->destroyAllEntities();

	m_boundary_system->init(*ecs(), m_event_bus.get());
	m_collision_system->init(*ecs(), m_event_bus.get());
	m_c_resolution_system->init(*ecs(), m_event_bus.get());

	m_state = std::move(state);
	m_state->setGame(this);
	m_state->start();
}

void Game::setState(std::unique_ptr<State> state, bool reset_camera, bool reset_bus, bool destroy_entities)
{
	if (reset_camera)
	{
		m_game_renderer->setGameCameraPosition(M_DEFAULT_GAME_CAMERA_POSITION);
	}
	if (reset_bus)
	{
		m_boundary_system->init(*ecs(), m_event_bus.get());
		m_collision_system->init(*ecs(), m_event_bus.get());
		m_c_resolution_system->init(*ecs(), m_event_bus.get());
	}

	if (destroy_entities)
	{
		ecs()->destroyAllEntities();
	}

	if (m_state != nullptr)
	{
		m_state->end();
	}

	m_state = std::move(state);
	m_state->setGame(this);
	m_state->start();
}

void Game::pauseState(std::unique_ptr<State> state)
{
	m_paused_game = true;
	assert(m_paused_state == nullptr && "state already paused");
	m_paused_state = std::move(m_state);
	m_state = std::move(state);
	m_state->setGame(this);
	m_state->start();
}

void Game::unpauseState()
{
	m_paused_game = false;
	m_state->end();
	m_state = std::move(m_paused_state);
	m_state->unpaused();
	m_paused_state = nullptr;
}

void Game::clearPauseState()
{
	m_paused_game = false;
	m_paused_state->end();
	m_paused_state = nullptr;
}

void Game::importLevel(const std::string& filepath, std::vector<ECS::Entity>& entity_list)
{
	std::ifstream in_file(filepath);
	assert(in_file.good() && "Failed to read level file on import.");
	std::stringstream str_stream;
	str_stream << in_file.rdbuf();

	// Check level node exists to say this is a level file.
	YAML::Node dat = YAML::Load(str_stream.str());
	if (!dat["Level"])
	{
		// TODO: Logging system.
#ifdef _DEBUG
		std::cout << "Error: Attempting to import a .LEVEL file with incorrect formatting.";
#endif
		return;
	}

	// Deserialize entities and their components. Load any asset resources required.
	auto entities = dat["Entities"];
	if (entities)
	{
		auto* p_ecs = ecs();
		for (const auto& entity_node : entities)
		{
			auto& deserialized_entity = entity_list.emplace_back(p_ecs->createEntity());

			// Transform component.
			const auto& transform_component = entity_node["TransformComponent"];
			TransformComponent transform;

			transform.m_position = transform_component["Position"].as<DirectX::SimpleMath::Vector2>();
			transform.m_scale = transform_component["Scale"].as<DirectX::SimpleMath::Vector2>();
			transform.m_rotation_radians = transform_component["RotationRadians"].as<float>();

			p_ecs->addComponent(deserialized_entity, transform);

			// Sprite component.
			const auto& sprite_component = entity_node["SpriteComponent"];
			if (sprite_component)
			{
				SpriteComponent sprite;

				sprite.m_relative_position = sprite_component["RelativePosition"].as<DirectX::SimpleMath::Vector2>();
				sprite.m_relative_rotation_radians = sprite_component["RelativeRotationRadians"].as<float>();
				sprite.m_relative_scale = sprite_component["RelativeScale"].as<DirectX::SimpleMath::Vector2>();
				sprite.m_colour = sprite_component["Colour"].as<DirectX::SimpleMath::Color>();
				sprite.m_z_order = sprite_component["ZOrder"].as<float>();

				auto texture_filepath = sprite_component["Texture"].as<std::string>();
				if (!texture_filepath.empty())
				{
					m_game_renderer->loadTexture(getDevice(), texture_filepath, sprite);
				}

				sprite.m_source_rect.top = sprite_component["SourceRectTop"].as<LONG>();
				sprite.m_source_rect.right = sprite_component["SourceRectRight"].as<LONG>();
				sprite.m_source_rect.bottom = sprite_component["SourceRectBottom"].as<LONG>();
				sprite.m_source_rect.left = sprite_component["SourceRectLeft"].as<LONG>();

				sprite.m_origin = sprite_component["Origin"].as<DirectX::SimpleMath::Vector2>();

				p_ecs->addComponent(deserialized_entity, sprite);
			}

			// Sprite anim component.
			const auto& sprite_anim_component = entity_node["SpriteAnimComponent"];
			if (sprite_anim_component)
			{
				SpriteAnimComponent sprite_anim;

				sprite_anim.m_relative_position = sprite_anim_component["RelativePosition"].as<DirectX::SimpleMath::Vector2>();
				sprite_anim.m_relative_rotation_radians = sprite_anim_component["RelativeRotationRadians"].as<float>();
				sprite_anim.m_relative_scale = sprite_anim_component["RelativeScale"].as<DirectX::SimpleMath::Vector2>();
				sprite_anim.m_colour = sprite_anim_component["Colour"].as<DirectX::SimpleMath::Color>();
				sprite_anim.m_z_order = sprite_anim_component["ZOrder"].as<float>();

				auto texture_filepath = sprite_anim_component["Texture"].as<std::string>();
				if (!texture_filepath.empty())
				{
					sprite_anim.m_texture_id = static_cast<int>(m_game_renderer->loadTexture(getDevice(), texture_filepath));
				}

				// Read frames.
				const YAML::Node& frames_node = sprite_anim_component["Frames"];
				for (size_t i = 0; i < frames_node.size(); i++)
				{
					RECT frame_rect;
					frame_rect.top = frames_node["Frame" + std::to_string(i)]["SourceRectTop"].as<LONG>();
					frame_rect.right = frames_node["Frame" + std::to_string(i)]["SourceRectRight"].as<LONG>();
					frame_rect.bottom = frames_node["Frame" + std::to_string(i)]["SourceRectBottom"].as<LONG>();
					frame_rect.left = frames_node["Frame" + std::to_string(i)]["SourceRectLeft"].as<LONG>();

					sprite_anim.m_frames.emplace_back(frame_rect);
				}

				sprite_anim.m_playback_rate_seconds = sprite_anim_component["PlaybackRate"].as<float>();
				sprite_anim.m_looping = sprite_anim_component["Looping"].as<bool>();
				sprite_anim.m_origin = sprite_anim_component["Origin"].as<DirectX::SimpleMath::Vector2>();

				p_ecs->addComponent(deserialized_entity, sprite_anim);
			}

			// Enemy spawner component.
			const auto& enemy_spawner_component = entity_node["EnemySpawnerComponent"];
			if (enemy_spawner_component)
			{
				EnemySpawnerData enemy_spawner;

				enemy_spawner.relative_position = enemy_spawner_component["RelativePosition"].as<DirectX::SimpleMath::Vector2>();
				enemy_spawner.type = static_cast<EnemyType>(enemy_spawner_component["EnemyType"].as<int>());
				enemy_spawner.spawn_count = enemy_spawner_component["SpawnCount"].as<int>();
				enemy_spawner.spawn_interval = enemy_spawner_component["SpawnInterval"].as<float>();

				p_ecs->addComponent(deserialized_entity, enemy_spawner);
			}

			// Environment tag.
			const auto& environment_tag = entity_node["EnvironmentTag"];
			if (environment_tag)
			{
				p_ecs->addComponent(deserialized_entity, EnvironmentTag());
			}

			// TODO: Deserialize other types of components.
		}
	}
}

void Game::setNumSelectedPlayers(int num_players) noexcept
{
	m_num_players_selected = num_players;
}

int Game::getNumSelectedPlayers() const noexcept
{
	return m_num_players_selected;
}

void Game::setLevelSelected(uint8_t level) noexcept
{
	m_level_selected = level;
}

uint8_t Game::getSelectedLevel() noexcept
{
	return m_level_selected;
}
