#pragma once

#include "pch.h"

struct LifetimeData
{
	float time;
	std::function<void()> onDeathCallback = nullptr;
};