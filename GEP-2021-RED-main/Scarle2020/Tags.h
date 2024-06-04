#pragma once

#include "pch.h"

// Type tags
struct PlayerTag {};
struct EnemyTag {};
//struct BossTag {};

// Behaviour Tags
struct InRangeTriggerTag {};
struct MoveTowardTag {};
struct MoveForwardTag {};
struct RotateTowardTag {};

struct HoldFireTag {};
struct StartFireTag {};

struct ChangeFireModeTag { ChangeFireModeTag() {} ChangeFireModeTag(unsigned mode) { m_mode = mode; } unsigned m_mode = 0; };
struct StartFixedPointShootingTag { StartFixedPointShootingTag() {} StartFixedPointShootingTag(unsigned mode) { m_mode = mode; } unsigned m_mode = 0; };
struct StopFixedPointShootingTag {};
struct SpawnADsTag { SpawnADsTag() {} SpawnADsTag(unsigned code) { m_code = code; } unsigned m_code = 0; };
struct WinConditionTag {};
//struct ResetRotationTag {};
