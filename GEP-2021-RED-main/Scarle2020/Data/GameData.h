#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_

//=================================================================
//Data to be passed by game to all ScarleGame Objects via Tick
//=================================================================

#include "GameState.h"

using namespace DirectX;

struct GameData
{
	float m_dt;  //time step since last frame
};
#endif
