#pragma once

#include "pch.h"
#include "BehaviourDefinitions.h"

struct StagedBehaviourComponent
{
	StagedBehaviourComponent() //Making sure all values are correctly initialized even if the array sizes are changed in BehaviourDefinitions
	{
		for (int i = 0; i < StagedBehaviour::MAX_BEHAVIOUR_TAGS; i++)
		{
			m_tags[i] = 0;
			m_constancy[i] = false;
			m_next_tag_timers[i] = 1.0F;
			m_tag_duration_timers[i] = 1.0F;
			m_current_tag_timers[i] = 0.0F;
		}
		for (int i = 0; i < StagedBehaviour::MAX_STAGES; i++)
		{
			m_health_triggers[i] = 0;
		}
	}
	unsigned m_tags[StagedBehaviour::MAX_BEHAVIOUR_TAGS] = {}; //System handles pairing numbers to tags
	unsigned m_tag_helper[StagedBehaviour::MAX_BEHAVIOUR_TAGS] = {}; //Currently only tag that needs it is the firing mode switcher, quite wasteful in memory, only storing tags in the component can eliminate this
	bool m_constancy[StagedBehaviour::MAX_BEHAVIOUR_TAGS] = {};
	float m_next_tag_timers[StagedBehaviour::MAX_BEHAVIOUR_TAGS] = {};
	float m_tag_duration_timers[StagedBehaviour::MAX_BEHAVIOUR_TAGS] = {};
	float m_current_tag_timers[StagedBehaviour::MAX_BEHAVIOUR_TAGS] = {}; //when reaches 0, system removes the tag
	float m_next_timer = 0.0F; //when reaches 0, add the next repeatable tag
	unsigned m_curr_tag_counter = 0;
	unsigned m_in_stage = 0;

	int m_health_triggers[StagedBehaviour::MAX_STAGES] = {}; //Made the stage trigger and behaviour system tightly coupled, could be split into two systems, or three with a timers system
};