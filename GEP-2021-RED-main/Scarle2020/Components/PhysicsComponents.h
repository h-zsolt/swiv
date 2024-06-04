#pragma once

#include "pch.h"

// Physics system example components.
// Component types definitions.
struct Gravity
{
    DirectX::XMFLOAT2 force = { 0.f, -2.f };
    Gravity() = default;
    Gravity(float strength) : force({ 0.f, strength }) {}
};

struct RigidBody
{
    DirectX::XMFLOAT2 velocity = { 0.f, 0.f };
    DirectX::XMFLOAT2 acceleration = { 0.f, 0.f };
};

struct Transform
{
    DirectX::XMFLOAT2 position = { 0.f, 0.f };
};
//////////////////////
