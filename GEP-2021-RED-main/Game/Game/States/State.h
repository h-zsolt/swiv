#pragma once

class Game;

struct GameData;

class State
{
public:
	State() = default;
	virtual ~State() = default;

	virtual void start()  = 0;
	virtual void updateObjects(GameData* gd) = 0;
	virtual void end()	  = 0;
	virtual void unpaused() {};

	void setGame(Game* game) { m_game = game; }

protected:
	Game* m_game{ nullptr };
};

