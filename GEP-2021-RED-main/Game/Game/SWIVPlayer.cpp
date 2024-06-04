#include "SWIVPlayer.h"

#include <ECS.h>
#include <Components/SpriteComponent.h>

SWIVPlayer::SWIVPlayer(ECS::Entity entity, int player_number) noexcept
	: m_entity(entity), m_player_number(player_number)
{}

void SWIVPlayer::makeVulnerable(ECS::ECS& ecs) noexcept
{
	// Set vulnerable flag. The player is vulnerable
	m_vulnerable = true;

	// Update sprite's alpha channel
	getSprite(ecs).m_colour.w = VULNERABLE_ALPHA;
}

void SWIVPlayer::makeInvulnerable(ECS::ECS& ecs, float time) noexcept
{
	// Clear vulnerable flag. The player is invulnerable
	m_vulnerable = false;

	// Set the time the player was made invulnerable
	m_last_hit_time = time;

	// Update sprite's alpha channel
	getSprite(ecs).m_colour.w = INVULNERABLE_ALPHA;
}

SpriteComponent& SWIVPlayer::getSprite(ECS::ECS& ecs) const noexcept
{
	return ecs.getComponent<SpriteComponent>(m_entity);
}
