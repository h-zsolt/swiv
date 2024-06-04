#pragma once

#include <pch.h>
#include <ECSDefinitions.h>
#include <EventBus.h>

#include "State.h"
#include "../SWIVPlayer.h"

namespace ECS { class ECS; }

struct GameEvent;

class InGame : public State
{
public:
	static constexpr int ENEMY_DESTROYED_SCORE_VALUE{ 1000 };
	static constexpr int ENEMY_SHOT_SCORE_VALUE{ 100 };
	static constexpr int NO_LIVES_REMAINING_AMOUNT{ 0 };
	static constexpr float PLAYER_INVULNERABLE_DURATION{ 3.F };
	static constexpr int P1_PLAYER_NUMBER{ 1 };
	static constexpr int P2_PLAYER_NUMBER{ 2 };
	static constexpr float EXPLOSION_PLAYBACK_RATE{ 0.015F };
	static constexpr size_t EXPLOSION_RESERVE_NUMBER{ 128 };
	DirectX::SimpleMath::Vector2 EXPLOSION_ENTITY_SCALE{ 2.F,2.F };

public:
	InGame() = default;
	~InGame() override = default;

	using PlayerIter = std::vector<SWIVPlayer>::iterator;
	using CPlayerIter = std::vector<SWIVPlayer>::const_iterator;

	void start() override;
	void updateObjects(GameData* gd) override;
	void end() override;

	void onGameEvent(GameEvent* game_event);

private:
	void createPlayers();
	void createHUD(ECS::ECS* p_ecs);

	void setHUDText(ECS::Entity hud_entity, const std::string& text) noexcept;

	void onPlayerHit(ECS::Entity player);
	void onPlayerDestroyed(CPlayerIter player) noexcept;
	void onEnemyShot(ECS::Entity player);
	void onEnemyDestroyed(ECS::Entity player, ECS::Entity enemy) noexcept;
	void onLevelEnd();
	void onGameOver();

	void hideGameHUD(ECS::ECS* p_ecs);

	ECS::Entity createHUDTextEntity(ECS::ECS* p_ecs,
		size_t font_id,
		const DirectX::SimpleMath::Vector2& position,
		const DirectX::SimpleMath::Vector2& relative_scale,
		const std::string& default_text = "HUD Text",
		bool visible = true);

	auto getPlayerFromEntity(ECS::Entity entity) noexcept;

	void playExplosion(const DirectX::SimpleMath::Vector2& position) noexcept;

private:
	// Data.
	std::vector<ECS::Entity> m_level_entity_list;
	std::vector<ECS::Entity> m_player_entity_list;
	std::vector<SWIVPlayer> m_player_list;
	std::vector<ECS::Entity> m_hud_entity_list;
	float m_elapsed_time{ 0.F };
	size_t m_explosion_texture_atlas_id{ 0 };
	std::vector<ECS::Entity> m_explosion_entity_list;
	static constexpr float M_BOUNDARY_WIDTH{ 1415.F };
	size_t m_num_starting_players{ 0 };

	// Level.
	bool m_reached_end{ false };
	bool m_check_enemy_system = false;

	int m_player_score{ 0 };
	int m_player_lives{ 3 };

	// Camera.
	static constexpr float CAMERA_SCROLL_SPEED{ 75.F };
	static constexpr float LEVEL_END_CAMERA_TARGET{ 3500.F };
	float m_current_camera_scroll_speed = CAMERA_SCROLL_SPEED;

	// Game HUD.
	DirectX::SimpleMath::Vector2 P1_SCORE_TEXT_POSITION{ 100.F, 15.F };
	DirectX::SimpleMath::Vector2 P1_SCORE_TEXT_REL_SCALE{ .65F, .65F };
	ECS::Entity m_p1_score_hud_entity{ 95718232 };

	DirectX::SimpleMath::Vector2 P1_LIVES_TEXT_POSITION{ 100.F, 80.F };
	DirectX::SimpleMath::Vector2 P1_LIVES_TEXT_REL_SCALE{ .65F, .65F };
	ECS::Entity m_p1_lives_hud_entity{ 95718233 };

	DirectX::SimpleMath::Vector2 P2_SCORE_TEXT_POSITION{ 1500.F, 15.F };
	DirectX::SimpleMath::Vector2 P2_SCORE_TEXT_REL_SCALE{ .65F, .65F };
	ECS::Entity m_p2_score_hud_entity{ 95718232 };

	DirectX::SimpleMath::Vector2 P2_LIVES_TEXT_POSITION{ 1500.F, 80.F };
	DirectX::SimpleMath::Vector2 P2_LIVES_TEXT_REL_SCALE{ .65F, .65F };
	ECS::Entity m_p2_lives_hud_entity{ 95718233 };

	DirectX::SimpleMath::Vector2 COMPLETE_MESSAGE_TEXT_POSITION{ 600.F, 340.F };
	DirectX::SimpleMath::Vector2 COMPLETE_MESSAGE_TEXT_REL_SCALE{ 1.F, 1.F };
	ECS::Entity m_level_complete_message_hud_entity{ 95718234 };

	DirectX::SimpleMath::Vector2 GAME_OVER_MESSAGE_TEXT_POSITION{ 600.F, 340.F };
	DirectX::SimpleMath::Vector2 M_GAME_OVER_MESSAGE_TEXT_REL_SCALE{ 1.F, 1.F };
	ECS::Entity m_game_over_message_hud_entity{ 95718235 };

	// Callbacks.
	CallbackID game_event_callback;

	//-------------------------------------------------
	ECS::Entity exp{ 934322 };
};