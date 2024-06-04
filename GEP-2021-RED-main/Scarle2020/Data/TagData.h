#pragma once

#include "pch.h"
#include <type_traits>

enum class TagID : int
{
    NULL_TAG = 0,
    PLAYER,
    ENEMY,
    P_BULLET,
    E_BULLET,
    ENVIRONMENT,
    POWER_UP
};

struct TagData
{
    TagID tag = TagID::NULL_TAG;
};


// --------------------------------------------------------------------------------------------- //
//                              Helper Functions & Operator Overloads                            //
// --------------------------------------------------------------------------------------------- //

inline TagID operator|(TagID lhs, TagID rhs)
{
	using U = std::underlying_type_t<TagID>;
	return static_cast<TagID>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

// cannot be used if lhs value is not intialized before use
inline TagID& operator|=(TagID& lhs, TagID&& rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

template<typename T>
inline TagID operator<<(T lhs, TagID rhs)
{
    assert(std::is_integral_v<T> && "T is not an integer type");
    using U  = std::underlying_type_t<TagID>;
    return static_cast<TagID>(static_cast<T>(lhs) << static_cast<U>(rhs));
}

namespace Bits
{
    // different template types allows for TagID operator << to be used
    template <typename L, typename R>
    static constexpr void addToBitFlag(L& lhs, R&& rhs)
    {
        lhs |= (1 << rhs);
    }
} // namespace Helper