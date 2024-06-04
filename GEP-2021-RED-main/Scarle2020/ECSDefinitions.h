#pragma once

#include "pch.h"

namespace ECS 
{
	using Entity = uint32_t;
	static const size_t MAX_ENTITIES = 5001;
	static const Entity INVALID_ENTITY = 0;

	using ComponentBitflag = uint8_t;
	static const ComponentBitflag MAX_COMPONENTS = 64;

	using Signature = std::bitset<MAX_COMPONENTS>;
}