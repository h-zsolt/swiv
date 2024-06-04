#include "pch.h"
#include "StagedBehaviourSystem.h"
#include "Components/StagedBehaviourComponent.h"
#include "BehaviourDefinitions.h"
#include "Data/HealthData.h"
#include "Tags.h"

#include "Components/TransformComponent.h" //this is just for a makeshift solution
#include "Data/RotationData.h"

void StagedBehaviourSystem::init(ECS::ECS& ecs)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<StagedBehaviourComponent>());
	ecs.setSystemSignature<StagedBehaviourSystem>(sig);
}

void StagedBehaviourSystem::update(ECS::ECS& ecs, const float dt)
{
	for (auto& entity : m_entities)
	{
		auto& behaviour = ecs.getComponent<StagedBehaviourComponent>(entity);
		auto& current_stage = behaviour.m_in_stage;
		auto stage_first_tag = current_stage * StagedBehaviour::MAX_TAGS_PER_STAGE;

		//Active Tag Handling
		for (int i = 0; i < StagedBehaviour::MAX_TAGS_PER_STAGE; i++) 
		{
			if (!behaviour.m_constancy[stage_first_tag+i]) //if they aren't constant, work with the timer
			{
				if (behaviour.m_current_tag_timers[stage_first_tag + i] > 0) //if currently active
				{
					behaviour.m_current_tag_timers[stage_first_tag + i] -= dt; //move down timer
					if (behaviour.m_current_tag_timers[stage_first_tag + i] <= 0)
					{
						removeTag(ecs, entity, behaviour.m_tags[stage_first_tag + i], behaviour.m_tag_helper[stage_first_tag + i]);
					}
				}
			}
		}

		//Next Tag Handling
		behaviour.m_next_timer -= dt;
		//When it's time to activate next tag, in a for loop so multiple tags can activate at the same time
		for (int i = 0; behaviour.m_next_timer <= 0 && i <  StagedBehaviour::MAX_TAGS_PER_STAGE; i++)
		{
			bool adding_flag_happened = false;
			for(int j = 0; !adding_flag_happened && j <  StagedBehaviour::MAX_TAGS_PER_STAGE; j++) //using 'for' loop instead of 'while' to make sure it won't get stuck
			{	//loops through the tags until it finds the first one that can be added, it can loop through all there is in a stage
				unsigned currently_checking = stage_first_tag + behaviour.m_curr_tag_counter; //prefer less calculations over memory use
				if (!behaviour.m_constancy[currently_checking] && behaviour.m_tags[currently_checking] != StagedBehaviour::EMPTY_BEHAVIOUR)
				{
					addTag(ecs, entity, behaviour.m_tags[currently_checking], behaviour.m_tag_helper[currently_checking]);
					adding_flag_happened = true;
					behaviour.m_next_timer = behaviour.m_next_tag_timers[currently_checking];
					behaviour.m_current_tag_timers[currently_checking] = behaviour.m_tag_duration_timers[currently_checking];
				}
				
				//Move to next tag
				behaviour.m_curr_tag_counter++;
				if (behaviour.m_curr_tag_counter >= StagedBehaviour::MAX_TAGS_PER_STAGE)
				{
					behaviour.m_curr_tag_counter = 0; //reset to the start of the stage's 
				}
			}
		}

		//Stage Handling
		if (ecs.hasComponent<HealthData>(entity))
		{
			if (ecs.getComponent<HealthData>(entity).m_current_health <= behaviour.m_health_triggers[behaviour.m_in_stage])
			{
				//Clear tags from current Stage
				for (int i = 0; i < StagedBehaviour::MAX_TAGS_PER_STAGE; i++)
				{
					if (!behaviour.m_constancy[stage_first_tag + i]) 
					{
						if (behaviour.m_current_tag_timers[stage_first_tag + i] > 0)
						{
							removeTag(ecs, entity, behaviour.m_tags[stage_first_tag + i], behaviour.m_tag_helper[stage_first_tag + i]);
						}
					}
					else
					{
						removeTag(ecs, entity, behaviour.m_tags[stage_first_tag + i], behaviour.m_tag_helper[stage_first_tag + i]);
					}
				}
				//Start next Stage
				current_stage++;
				stage_first_tag = current_stage * StagedBehaviour::MAX_TAGS_PER_STAGE;
				for (int i = 0; i < StagedBehaviour::MAX_TAGS_PER_STAGE; i++)
				{
					if (behaviour.m_constancy[stage_first_tag + i])
					{
						addTag(ecs, entity, behaviour.m_tags[stage_first_tag + i], behaviour.m_tag_helper[stage_first_tag + i]);
					}
				}
				//Cleanup
				behaviour.m_curr_tag_counter = 0;
				behaviour.m_next_timer = 0;
			}
		}
	}
}

void StagedBehaviourSystem::addTag(ECS::ECS& ecs, ECS::Entity entity, unsigned target_behaviour, unsigned behaviour_helper)
{
	switch (target_behaviour)
	{
	case StagedBehaviour::MOVE_TOWARD_TAG:
		ecs.addComponent(entity, MoveTowardTag());
		break;
	case StagedBehaviour::ROTATE_TOWARD_TAG:
	{
		RotationData rotation;
		rotation.speed = 8.0F + behaviour_helper;
		ecs.addComponent(entity, rotation);
		ecs.addComponent(entity, RotateTowardTag());
		break;
	}
	case StagedBehaviour::HOLD_FIRE_TAG:
		ecs.addComponent(entity, HoldFireTag());
		break;
	case StagedBehaviour::START_FIRE_TAG:
		ecs.addComponent(entity, StartFireTag());
		break;
	case StagedBehaviour::TEMPORARY_FIRE_TAGS:
		ecs.addComponent(entity, StartFireTag());
		break;
	case StagedBehaviour::CHANGE_FIRING_MODE:
		ecs.addComponent(entity, ChangeFireModeTag(behaviour_helper));
		break;
	case StagedBehaviour::RESET_ROTATION:
		//didn't want to mess with TargetData and rotation maths are difficult, so just a makeshift thing
		if (ecs.hasComponent<TransformComponent>(entity))
		{
			ecs.getComponent<TransformComponent>(entity).m_rotation_radians = 3.14F; 
			//ecs.getComponent<TransformComponent>(entity).m_rotation_radians = 0;
		}
		break;
	case StagedBehaviour::SPAWN_ADS_TAG:
		ecs.addComponent(entity, SpawnADsTag(behaviour_helper));
		//UNDONE: moved to removeTag so it can be timed when the first one spawns even without implementing in system
		//This made ads spawn when swapping stages, whoops
		break;
	default:
		break;
	}
}

void StagedBehaviourSystem::removeTag(ECS::ECS& ecs, ECS::Entity entity, unsigned target_behaviour, unsigned behaviour_helper)
{
	switch (target_behaviour)
	{
	case StagedBehaviour::MOVE_TOWARD_TAG:
		ecs.removeComponent<MoveTowardTag>(entity);
		break;
	case StagedBehaviour::ROTATE_TOWARD_TAG:
		ecs.removeComponent<RotateTowardTag>(entity);
		ecs.removeComponent<RotationData>(entity);
		break;
	case StagedBehaviour::HOLD_FIRE_TAG:
		//ecs.removeComponent<HoldFireTag>(entity);
		//System handles removal of tag
		break;
	case StagedBehaviour::START_FIRE_TAG:
		ecs.addComponent(entity, HoldFireTag()); //turned it into the same as temp fire, because even constant firing is better handled with it
		//ecs.removeComponent<StartFireTag>(entity);
		//System handles removal of tag
		break;
	case StagedBehaviour::TEMPORARY_FIRE_TAGS:
		ecs.addComponent(entity, HoldFireTag());
		break;
	case StagedBehaviour::CHANGE_FIRING_MODE:
		//System handles removal of tag
		break;
	case StagedBehaviour::SPAWN_ADS_TAG:
		//ecs.addComponent(entity, SpawnADsTag(behaviour_helper));
		//system handles removal of tag
		break;
	default:
		break;
	}
}
