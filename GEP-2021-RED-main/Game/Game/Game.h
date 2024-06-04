#pragma once

#include <ScarleGame.h>
#include <ECS.h>

#include <Game/States/State.h>
#include "GameRenderer.h"
#include "EventBus.h"
#include <InRangeTrigger.h>

#include <Game/States/InMenu.h>

class InputSystem;
class MovementSystem;
class RotationSystem;
class BoundarySystem;
class CollisionSystem;
class CollisionResolutionSystem;
class ColliderMovementSystem;
class BulletSpawnerSystem;
class EnemySpawnerSystem;
class PlayerController;
class LifetimeSystem;
class TargetingSystem;
class MoveTowardSystem;
class MoveForwardSystem;
class RotateTowardSystem;
class HealthBarSystem;
class HealthSystem;
class EnvironmentSpawnSystem;
class PowerUpSystem;
class StagedBehaviourSystem;
class FireControlSystem;
class FiringModeSystem;
class FixedShootingPointSystem;
class ADSpawnerSystem;
class NoEnemiesWinSystem;

class Game final : public ScarleGame
{
public:
	Game() noexcept;
	~Game() final = default;

	void start() override;
	void updateObjects(GameData* gd) override;
	void renderObjects() override;

	// Messages
	void onActivated() override;
	void onDeactivated() override;
	void onSuspending() override;
	void onResuming() override;
	void onWindowSizeChanged(int _width, int _height) override;

	DirectX::SimpleMath::Vector2 getWindowDimensions() const { return getDimensions(); }
	
	// Set State
	void setState(std::unique_ptr<State> state);
	void setState(std::unique_ptr<State> state, bool reset_camera, bool reset_bus, bool destroy_entities);
	void pauseState(std::unique_ptr<State> state);
	void unpauseState();
	void clearPauseState();
	 // bool m_paused = false;

	// States
	std::unique_ptr<InMenu> m_menu_state = nullptr;

	GameRenderer* getGameRenderer() const { return m_game_renderer.get(); }
	InputSystem* getInputSystem() const { return m_input_system; }
	BoundarySystem* getBoundarySystem() const { return m_boundary_system; }
	CollisionSystem* getCollisionSystem() const { return m_collision_system; }
	CollisionResolutionSystem* getCollisionResolutionSystem() const { return m_c_resolution_system; }
	ColliderMovementSystem* getColliderMovementSystem() const { return m_collider_movement_system; }
	PlayerController* getPlayerController() const { return m_player_controller; }
	BulletSpawnerSystem* getBulletSpawner() const { return m_bullet_spawner; }
	EnemySpawnerSystem* getEnemySpawner() const { return m_enemy_spawner; }
	TargetingSystem* getTargetingSystem() { return m_targeting_system; }
	EventBus* getEventBus() { return m_event_bus.get(); }
	EnvironmentSpawnSystem* getEnvironmentSpawner() const { return m_environment_spawn_system; }
	PowerUpSystem* getPowerUpSystem() const { return m_power_up_system; }
	HealthSystem* getHealthSystem() const { return m_health_system; }
	HealthBarSystem* getHealthBarSystem() const { return m_healthbar_system; }
	FireControlSystem* getFireControlSystem() const { return m_fire_control_system; }
	FiringModeSystem* getFiringModeSystem() const { return m_firing_mode_system; }
	FixedShootingPointSystem* getFixedPointShootingSystem() const { return m_fixed_shooting_point_system; }
	ADSpawnerSystem* getADSpawnerSystem() const { return m_ad_spawner_system; }
	NoEnemiesWinSystem* getNoEnemiesWinSystem() const { return m_no_enemies_win_system; }
	MovementSystem* getMovementSystem() const { return m_movement_system; }
	RotationSystem* getRotationSystem() const { return m_rotation_system; }
	BoundarySystem* getBoundrySystem() const { return m_boundary_system; }

	void importLevel(const std::string& filepath, std::vector<ECS::Entity>& entity_list);

	void setNumSelectedPlayers(int num_players) noexcept;
	int getNumSelectedPlayers() const noexcept;

	void setLevelSelected(uint8_t level) noexcept;
	uint8_t getSelectedLevel() noexcept;

private:
	// Internals.
	std::unique_ptr<State> m_state;
	std::unique_ptr<State> m_paused_state = nullptr;
	bool m_paused_game = false;
	std::unique_ptr<GameRenderer> m_game_renderer;
	std::unique_ptr<EventBus> m_event_bus;

	// Behaviour Systems
	InputSystem* m_input_system                = nullptr;
	PlayerController* m_player_controller      = nullptr;
	
	InRangeTriggerSystem* m_in_range_trigger_system = nullptr;
	MoveTowardSystem* m_move_toward_system     = nullptr;
	MoveForwardSystem* m_move_forward_system    = nullptr;
	RotateTowardSystem* m_rotate_toward_system = nullptr;
	
	///
	LifetimeSystem* m_lifetime_system          = nullptr;
	TargetingSystem* m_targeting_system        = nullptr;
	BulletSpawnerSystem* m_bullet_spawner      = nullptr;
	EnemySpawnerSystem* m_enemy_spawner        = nullptr;

	MovementSystem*       m_movement_system   = nullptr;
	RotationSystem*       m_rotation_system   = nullptr;

	BoundarySystem* m_boundary_system							= nullptr;
	CollisionSystem* m_collision_system							= nullptr;
	CollisionResolutionSystem* m_c_resolution_system			= nullptr;
	ColliderMovementSystem* m_collider_movement_system			= nullptr;
	EnvironmentSpawnSystem* m_environment_spawn_system			= nullptr;

	StagedBehaviourSystem* m_staged_behaviour_system = nullptr;
	HealthBarSystem* m_healthbar_system = nullptr;
	HealthSystem* m_health_system = nullptr;
	PowerUpSystem* m_power_up_system = nullptr;

	FiringModeSystem* m_firing_mode_system = nullptr;
	FireControlSystem* m_fire_control_system = nullptr;
	FixedShootingPointSystem* m_fixed_shooting_point_system = nullptr;
	ADSpawnerSystem* m_ad_spawner_system = nullptr;
	NoEnemiesWinSystem* m_no_enemies_win_system = nullptr;

	// Boundary width value.
	static constexpr float M_BOUNDARY_WIDTH{ 1415.F };

	// Camera.
	DirectX::SimpleMath::Vector3 M_DEFAULT_GAME_CAMERA_POSITION{ 0.0f, 0.0f, 0.0f };

	// State communication
	int m_num_players_selected{ 0 };
	uint8_t m_level_selected{ 0 };
};
