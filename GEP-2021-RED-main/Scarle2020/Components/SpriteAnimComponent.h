#pragma once

#include "pch.h"

// The animation must be contained in a single texture. Expects a 
// spritesheet to be used.
struct SpriteAnimComponent
{
	DirectX::SimpleMath::Vector2 m_relative_position;
	float m_relative_rotation_radians = 0.f;
	DirectX::SimpleMath::Vector2 m_relative_scale = { 1.f, 1.f };
	DirectX::SimpleMath::Vector2 m_origin;
	DirectX::SimpleMath::Color m_colour = { 1.f, 1.f, 1.f, 1.f };
	// Drawn from highest to lowest meaning 0.0 will be on top.
	float m_z_order = 0.f;
	int m_texture_id = -1;
	std::vector<RECT> m_frames;
	float m_playback_rate_seconds = 1.f;
	bool m_looping = false;

	// Not user edited.
	size_t m_current_frame = 0;
	// Not user edited.
	bool m_completed = false;
	// Not user edited.
	float m_last_frame_increment_time = 0.f;
};