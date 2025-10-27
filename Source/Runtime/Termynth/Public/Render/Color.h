#pragma once
#include "Core/Core.h"
#include "Core/Math/Math.h"
#include <array>

enum class EColorAnsi16 : uint8
{
    Black = 0,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Gray,
    Ruby,
    Lime,
    Gold,
    Diamond,
    Fuchsia,
    Aqua,
    ReallyWhite
};

constexpr Vector operator+(EColorAnsi16 color)
{
    constexpr std::array<Vector,16> table = {{
        {0,0,0},        // 0
        {128,0,0},      // 1
        {0,128,0},      // 2
        {128,128,0},    // 3
        {0,0,128},      // 4
        {128,0,128},    // 5
        {0,128,128},    // 6
        {192,192,192},  // 7
        {128,128,128},  // 8
        {255,0,0},      // 9
        {0,255,0},      // 10
        {255,255,0},    // 11
        {0,0,255},      // 12
        {255,0,255},    // 13
        {0,255,255},    // 14
        {255,255,255}   // 15
    }};
    return table[static_cast<uint8_t>(color)];
}

namespace Color
{
    uint8 ColorComponentToBinary(uint8 value);
    /**
    * @brief Converts an RGB color to an ANSI color base code.
    *
    * @return int32 The corresponding ANSI color base code (0-15)
    */
    int32 RGBToAnsiCode(uint8 r, uint8 g, uint8 b);

    /**
    * @brief Converts base code to the ANSI code
    *
    * @return int32 The corresponding ANSI color code (30-37, 40-47, 90-97, 100-107)
    */
    int32 MapBaseCodeToSGR(uint8 baseCode, bool bBackground);
}
