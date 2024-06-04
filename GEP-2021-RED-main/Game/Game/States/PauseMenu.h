#pragma once
#include "State.h"
#include "keyboard.h"
class PauseMenu :
    public State
{
public:
	PauseMenu() = default;
	~PauseMenu() override = default;

	void start() override;
	void updateObjects(GameData * gd) override;
	void end() override;
private:
	bool loadSprite(const std::string& texture_path,
		ECS::Entity entity,
		const std::pair<float, float>& transform,
		const std::pair<float, float>& relative_pos,
		const std::pair<float,
		float>& scale, const DirectX::SimpleMath::Color& color,
		float z_order);
	void keyboardInput();
	void selectOption();
	enum class MenuOption
	{
		CONTROLS = 0,
		EXIT 
	}m_selected_option = MenuOption::CONTROLS;

	std::vector<ECS::Entity> m_entities;
	bool m_w_down = false;
	bool m_s_down = false;
	bool m_enter_down = true;
};

