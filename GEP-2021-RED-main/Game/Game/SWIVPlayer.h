#pragma once

#include <ECSDefinitions.h>
#include <OOP/DefaultPlayer.h>

namespace ECS { class ECS; }
struct SpriteComponent;

class SWIVPlayer : public DefaultPlayer
{
public:
	static constexpr float VULNERABLE_ALPHA{ 1.0F };
	static constexpr float INVULNERABLE_ALPHA{ 0.6F };
	static constexpr int STARTING_LIVES{ 3 };
	static constexpr int STARTING_SCORE{ 0 };

public:
	SWIVPlayer(ECS::Entity entity, int player_number) noexcept;
	~SWIVPlayer() noexcept = default;

	SWIVPlayer(SWIVPlayer&& rhs) noexcept = default;
	SWIVPlayer& operator=(SWIVPlayer&& rhs) noexcept = default;

	SWIVPlayer(const SWIVPlayer& rhs) noexcept = default;
	SWIVPlayer& operator=(const SWIVPlayer& rhs) noexcept = default;

	constexpr ECS::Entity entity() const noexcept { return m_entity; }

	constexpr int lives() const noexcept { return m_lives; }
	std::string getLivesHUDText() const noexcept 
	{
		return "P" + std::to_string(m_player_number) + " Lives: " + std::to_string(m_lives);
	}

	constexpr void setLives(int lives) noexcept { m_lives = lives; }
	constexpr void addSingleLife() noexcept { ++m_lives; }
	constexpr void removeSingleLife() noexcept { --m_lives; }

	constexpr bool vulnerable() const noexcept { return m_vulnerable; }

	constexpr float lastHitTime() const noexcept { return m_last_hit_time; }

	constexpr int score() const noexcept { return m_score; }
	constexpr void setScore(int score) noexcept { m_score = score >= 0 ? score : 0; }
	constexpr void addScore(int amount) noexcept { m_score += amount; }
	std::string getScoreHUDText() const noexcept
	{
		return "P" + std::to_string(m_player_number) + " Score: " + std::to_string(m_score);
	}

	constexpr int playerNumber() const noexcept { return m_player_number; }

	void makeVulnerable(ECS::ECS& ecs) noexcept;
	void makeInvulnerable(ECS::ECS& ecs, float time) noexcept;

private:
	SpriteComponent& getSprite(ECS::ECS& ecs) const noexcept;

private:
	ECS::Entity m_entity;
	int m_player_number;
	bool m_vulnerable{ true };
	float m_last_hit_time{ 0.F };
	int m_lives{ STARTING_LIVES };
	int m_score{ STARTING_SCORE };
};
