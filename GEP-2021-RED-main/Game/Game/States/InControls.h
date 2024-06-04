#pragma once
#include "State.h"
#include "Keyboard.h"
class InControls : public State
{
public:
	InControls() = default;
	~InControls() override = default;

	void start() override;
	void updateObjects(GameData* gd) override;
	void end() override;
	

private:
	enum class KeyBindingsSelector
	{
		NO_ID = -1,
		BACK = 0,
		P1_UP,
		P1_DOWN,
		P1_LEFT,
		P1_RIGHT,
		P1_SELECT,
		P1_BACK,
		P1_FIRE,
		P1_POWERUP,
		P2_UP,
		P2_DOWN,
		P2_LEFT,
		P2_RIGHT,
		P2_SELECT,
		P2_BACK,
		P2_FIRE,
		P2_POWERUP
	}m_selected_binding = KeyBindingsSelector::BACK;

	bool loadSprite(const std::string& texture_path,
		ECS::Entity entity,
		const std::pair<float, float>& transform,
		const std::pair<float, float>& relative_pos,
		const std::pair<float,
		float>& scale, const DirectX::SimpleMath::Color& color,
		float z_order);
	KeyBindingsSelector changeSelected(KeyBindingsSelector current_selection, int i);
	void selectBindings();
	void changeBinding();
	void setBinding(DirectX::Keyboard::Keys key, KeyBindingsSelector binding);
	std::string findKeyInKeys(DirectX::Keyboard::Keys key);
	void playMenuSound();

	bool m_changing_key = false;
	std::vector<std::pair<ECS::Entity,KeyBindingsSelector>> m_entities;
	using Keyboard = DirectX::Keyboard;
	std::array<std::pair<DirectX::Keyboard::Keys,std::string>, 48> m_keys = { std::make_pair(Keyboard::Keys::A,"A"),std::make_pair(Keyboard::Keys::B,"B"),std::make_pair(Keyboard::Keys::C,"C"),std::make_pair(Keyboard::Keys::D,"D"),
													std::make_pair(Keyboard::Keys::E,"E"),std::make_pair(Keyboard::Keys::F,"F"),std::make_pair(Keyboard::Keys::G,"G"),std::make_pair(Keyboard::Keys::H,"H"),
													std::make_pair(Keyboard::Keys::I,"I"),std::make_pair(Keyboard::Keys::J,"J"),std::make_pair(Keyboard::Keys::K,"K"),std::make_pair(Keyboard::Keys::L,"L"),
													std::make_pair(Keyboard::Keys::M,"M"),std::make_pair(Keyboard::Keys::N,"N"),std::make_pair(Keyboard::Keys::O,"O"),std::make_pair(Keyboard::Keys::P,"P"),
													std::make_pair(Keyboard::Keys::Q,"Q"),std::make_pair(Keyboard::Keys::R,"R"),std::make_pair(Keyboard::Keys::S,"S"),std::make_pair(Keyboard::Keys::T,"T"),
													std::make_pair(Keyboard::Keys::U,"U"),std::make_pair(Keyboard::Keys::V,"V"),std::make_pair(Keyboard::Keys::W,"W"),std::make_pair(Keyboard::Keys::X,"X"),
													std::make_pair(Keyboard::Keys::Y,"Y"),std::make_pair(Keyboard::Keys::Z,"Z"),std::make_pair(Keyboard::Keys::Space,"SPACE"),std::make_pair(Keyboard::Keys::NumPad0,"NUM 0"),
													std::make_pair(Keyboard::Keys::NumPad1,"NUM 1"),std::make_pair(Keyboard::Keys::NumPad2,"NUM 2"),std::make_pair(Keyboard::Keys::NumPad3,"NUM 3"),std::make_pair(Keyboard::Keys::NumPad4,"NUM 4"),
													std::make_pair(Keyboard::Keys::NumPad5,"NUM 5"),std::make_pair(Keyboard::Keys::NumPad6,"NUM 6"),std::make_pair(Keyboard::Keys::NumPad7,"NUM 7"),std::make_pair(Keyboard::Keys::NumPad8,"NUM 8"),
													std::make_pair(Keyboard::Keys::NumPad9,"NUM 9"),std::make_pair(Keyboard::Keys::Multiply,"NUM *"),std::make_pair(Keyboard::Keys::Add,"NUM +"),
													std::make_pair(Keyboard::Keys::Subtract,"NUM -"),std::make_pair(Keyboard::Keys::Decimal,"NUM ."),std::make_pair(Keyboard::Keys::Divide,"NUM /"),std::make_pair(Keyboard::Keys::LeftShift,"LEFT SHIFT"),
													std::make_pair(Keyboard::Keys::RightShift,"RIGHT SHIFT"),std::make_pair(Keyboard::Keys::LeftControl,"LEFT CTRL"),std::make_pair(Keyboard::Keys::RightControl,"RIGHT CTRL"),std::make_pair(Keyboard::Keys::LeftAlt,"LEFT ALT"),
													std::make_pair(Keyboard::Keys::RightAlt,"RIGHT ALT") };
};
