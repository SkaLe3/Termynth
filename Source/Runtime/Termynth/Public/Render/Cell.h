#pragma once
#include "Render/CellAttribute.h"
#include "Render/Color.h"
#include "Core/Math/Math.h"
#include <array>

struct alignas(8) Cell
{
    uint8 Char[2];
    ECellAttribute Attributes;
    uint8 FgR, FgG, FgB;
    uint8 BgR, BgG, BgB;

    void AddAttribute(ECellAttribute inAttr) {Attributes |= inAttr;}
    void RemoveAttribute(ECellAttribute inAttr) {Attributes &= ~inAttr;}
    bool HasAttribute(ECellAttribute inAttr) const {return CellAttribute::HasFlag(Attributes, inAttr);}

    void SetFgColor(uint8 r, uint8 g, uint8 b) {FgR = r; FgG = g, FgB = b;}
    void SetFgColor(const Vector& color) {FgR = color.x; FgG = color.y, FgB = color.z;}
    void SetFgColor(EColorAnsi16 color) {SetFgColor(+color);}
    void SetFgColor(const std::array<uint8, 3>& color) {SetFgColor(color[0], color[1], color[2]); }

    void SetBgColor(uint8 r, uint8 g, uint8 b) {BgR = r; BgG = g, BgB = b;}
    void SetBgColor(const Vector& color) {BgR = color.x; BgG = color.y, BgB = color.z;}
    void SetBgColor(EColorAnsi16 color) {SetBgColor(+color);}
    void SetBgColor(const std::array<uint8, 3>& color) {SetBgColor(color[0], color[1], color[2]); }

    static bool MatchPropertiesExact(const Cell& lhs, const Cell& rhs);
    static bool MatchPropertiesMapped(const Cell& lhs, const Cell& rhs);
    static bool MatchAttributes(const Cell& lhs, const Cell& rhs);
    static bool MatchFgColorExact(const Cell& lhs, const Cell& rhs);
    static bool MatchFgColorMapped(const Cell& lhs, const Cell& rhs);
    static bool MatchBgColorExact(const Cell& lhs, const Cell& rhs);
    static bool MatchBgColorMapped(const Cell& lhs, const Cell& rhs);
    static bool EqualMapped(const Cell& lhs, const Cell& rhs);
    
    static uint8 MapAnsi16(uint8 value);
};

bool operator==(const Cell& lhs, const Cell& rhs);
bool operator!=(const Cell& lhs, const Cell& rhs);
