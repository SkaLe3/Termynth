#pragma once
#include "Core/Core.h"

#include <type_traits>

enum class ECellAttribute : uint8
{
    None = 0,
    Bold = 1 << 0,
    Dim = 1 << 1,
    Italic = 1 << 2,
    Underline = 1 << 3,
    Blink = 1 << 4,
    Inverse = 1 << 5,
    DefaultFg = 1 << 6,
    DefaultBg = 1 << 7,

    MAX = Inverse
};

inline ECellAttribute operator|(ECellAttribute lhs, ECellAttribute rhs)
{
    return static_cast<ECellAttribute>(
        static_cast<std::underlying_type_t<ECellAttribute>>(lhs) |
        static_cast<std::underlying_type_t<ECellAttribute>>(rhs));
}

inline ECellAttribute &operator|=(ECellAttribute &lhs, ECellAttribute rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

inline ECellAttribute operator&(ECellAttribute lhs, ECellAttribute rhs)
{
    return static_cast<ECellAttribute>(
        static_cast<std::underlying_type_t<ECellAttribute>>(lhs) &
        static_cast<std::underlying_type_t<ECellAttribute>>(rhs));
}

inline ECellAttribute &operator&=(ECellAttribute &lhs, ECellAttribute rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

inline ECellAttribute operator~(ECellAttribute attr)
{
    return static_cast<ECellAttribute>(
        ~static_cast<std::underlying_type_t<ECellAttribute>>(attr));
}

namespace CellAttribute
{
    constexpr uint8 AttributeMasks[] = {
        static_cast<uint8>(ECellAttribute::Bold),
        static_cast<uint8>(ECellAttribute::Dim),
        static_cast<uint8>(ECellAttribute::Italic),
        static_cast<uint8>(ECellAttribute::Underline),
        static_cast<uint8>(ECellAttribute::Blink),
        static_cast<uint8>(ECellAttribute::Inverse),
    };

    constexpr int32 AttrOn[] = {1, 2, 3, 4, 5, 7};
    constexpr int32 AttrOff[] = {22, 22, 23, 24, 25, 27};

    inline bool HasFlag(ECellAttribute value, ECellAttribute test)
    {
        return (static_cast<std::underlying_type_t<ECellAttribute>>(value) &
                static_cast<std::underlying_type_t<ECellAttribute>>(test)) != 0;
    }

    inline ECellAttribute Difference(ECellAttribute lhs, ECellAttribute rhs)
    {
        return static_cast<ECellAttribute>(static_cast<uint8>(lhs) ^ static_cast<uint8>(rhs));
    }

    inline ECellAttribute Mask(ECellAttribute value, ECellAttribute mask)
    {
        return static_cast<ECellAttribute>(static_cast<uint8>(value) & static_cast<uint8>(mask));
    }

    inline uint8 MaskUint8(ECellAttribute value, ECellAttribute mask)
    {
        return (static_cast<uint8>(value) & static_cast<uint8>(mask));
    }

    inline bool MatchDefaultFg(ECellAttribute lhs, ECellAttribute rhs)
    {
        return  MaskUint8(lhs, ECellAttribute::DefaultFg) ==
                MaskUint8(rhs, ECellAttribute::DefaultFg);
    }

    inline bool MatchDefaultBg(ECellAttribute lhs, ECellAttribute rhs)
    {
        return  MaskUint8(lhs, ECellAttribute::DefaultBg) ==
                MaskUint8(rhs, ECellAttribute::DefaultBg);
    }

    inline int32 AttributeToAnsiCode(ECellAttribute attr)
    {
        switch (attr)
        {
        case ECellAttribute::Bold:
            return 1;
        case ECellAttribute::Dim:
            return 2;
        case ECellAttribute::Italic:
            return 3;
        case ECellAttribute::Underline:
            return 4;
        case ECellAttribute::Blink:
            return 5;
        case ECellAttribute::Inverse:
            return 7;
        default:
            return -1;
        }
    }

    inline int32 AttributeToAnsiCodeReset(ECellAttribute attr)
    {
        switch (attr)
        {
        case ECellAttribute::Bold:
        case ECellAttribute::Dim:
            return 22;
        case ECellAttribute::Italic:
            return 23;
        case ECellAttribute::Underline:
            return 24;
        case ECellAttribute::Blink:
            return 25;
        case ECellAttribute::Inverse:
            return 27;
        default:
            return -1;
        }
    }
}
