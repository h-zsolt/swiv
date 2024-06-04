#pragma once

#include "State.h"
#include "Keyboard.h"

class InMenu : public State
{
public:
	InMenu() = default;
	~InMenu() override = default;

	void start() override;
	void updateObjects(GameData* gd) override;
	void end() override;
	void unpaused() override;

private:
	enum class Levels : uint8_t
	{
		LEVEL1 = 1,
		LEVEL2,
		LEVEL3,
		LEVEL4
	};

	bool getKeyboardData();
	bool loadSprite(const std::string& texture_path, 
		ECS::Entity entity, 
		const std::pair<float, float>& transform, 
		const std::pair<float, float>& relative_pos,
		const std::pair<float, 
		float>& scale,
		const DirectX::SimpleMath::Color& color,
		float z_order,
		bool visible = true);
	bool loadNextScreen();
	void playMenuSound();
	enum class MenuItems
	{
		START = 0,
		CONTROLS = 1,
		PLAYERS = 2,
		LEVELS = 3,
		EXIT = 4
	} m_item_selected = MenuItems::START;
	MenuItems incrementItem(MenuItems current_item, int i);
	ECS::Entity m_start_entity;
	ECS::Entity m_controls_entity;
	ECS::Entity m_players_entity;

	void switchLevelThumb(ECS::Entity from, ECS::Entity to) noexcept;
	ECS::Entity m_level1_entity;
	ECS::Entity m_level2_entity;
	ECS::Entity m_level3_entity;
	ECS::Entity m_level4_entity;
	Levels m_selected_level{ Levels::LEVEL1 };

	ECS::Entity m_exit_entity;
	ECS::Entity m_select_entity;
	uint8_t players = 0;
	std::array<std::pair<float, float>, 3> m_select_positions{ std::pair<float,float>{1200.f,500.f},std::pair<float,float>{1200.f,700.f},std::pair<float,float>{1200.f,900.f} };
};
