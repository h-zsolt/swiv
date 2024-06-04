#pragma once

#include "pch.h"

namespace StagedBehaviour
{
	enum StagedBehaviourIDs
	{
		EMPTY_BEHAVIOUR,
		MOVE_TOWARD_TAG,
		ROTATE_TOWARD_TAG,
		HOLD_FIRE_TAG,
		START_FIRE_TAG, //same as temp fire tags
		TEMPORARY_FIRE_TAGS,
		CHANGE_FIRING_MODE,
		SHOOT_FIXED_POINT,
		RESET_ROTATION,
		SPAWN_ADS_TAG
	};
	static const int MAX_TAGS_PER_STAGE = 10;
	static const int MAX_STAGES = 5;
	static const int MAX_BEHAVIOUR_TAGS = MAX_STAGES * MAX_TAGS_PER_STAGE;
	static const int MAX_FIRING_MODES = 5;
	static const int MAX_ADD_SHOOTING_POINTS = 10;
	static const int MAX_AD_ENEMY_SPAWNERS = 10;
}
