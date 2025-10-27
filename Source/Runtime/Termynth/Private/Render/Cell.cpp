#include "Render/Cell.h"

bool Cell::MatchPropertiesExact(const Cell& lhs, const Cell& rhs)
{
    return 
    (
        MatchAttributes(lhs, rhs) &&
        MatchFgColorExact(lhs, rhs) &&
        MatchBgColorExact(lhs, rhs)
    );
}


bool Cell::MatchPropertiesMapped(const Cell& lhs, const Cell& rhs)
{
    return 
    (
        MatchAttributes(lhs, rhs) &&
        MatchFgColorMapped(lhs, rhs) &&
        MatchBgColorMapped(lhs, rhs)
    );
}

bool Cell::MatchAttributes(const Cell& lhs, const Cell& rhs)
{
    return lhs.Attributes == rhs.Attributes;
}

bool Cell::MatchFgColorExact(const Cell& lhs, const Cell& rhs)
{
    return 
    (
        lhs.FgR == rhs.FgR &&
        lhs.FgG == rhs.FgG &&
        lhs.FgB == rhs.FgB 
    );
}

bool Cell::MatchFgColorMapped(const Cell& lhs, const Cell& rhs)
{
    return 
    (
        MapAnsi16(lhs.FgR) == MapAnsi16(rhs.FgR) &&
        MapAnsi16(lhs.FgG) == MapAnsi16(rhs.FgG) &&
        MapAnsi16(lhs.FgB) == MapAnsi16(rhs.FgB) 
    );
}

bool Cell::MatchBgColorExact(const Cell& lhs, const Cell& rhs)
{
    return 
    (
        lhs.BgR == rhs.BgR &&
        lhs.BgG == rhs.BgG &&
        lhs.BgB == rhs.BgB 
    );
}

bool Cell::MatchBgColorMapped(const Cell& lhs, const Cell& rhs)
{
    return 
    (
        MapAnsi16(lhs.BgR) == MapAnsi16(rhs.BgR) &&
        MapAnsi16(lhs.BgG) == MapAnsi16(rhs.BgG) &&
        MapAnsi16(lhs.BgB) == MapAnsi16(rhs.BgB) 
    );
}

bool Cell::EqualMapped(const Cell& lhs, const Cell& rhs)
{
    return lhs.Char == rhs.Char && MatchPropertiesMapped(lhs, rhs);
}

uint8 Cell::MapAnsi16(uint8 value)
{
    if (value < 64) return 0;
    if (value < 160) return 128;
    if (value < 224) return 192;
    return 255;
}

bool operator==(const Cell& lhs, const Cell& rhs)
{
    return 
    (   
        lhs.Char == rhs.Char &&
        lhs.Attributes == rhs.Attributes &&
        lhs.FgR == rhs.FgR &&
        lhs.FgG == rhs.FgG &&
        lhs.FgB == rhs.FgB &&
        lhs.BgR == rhs.BgR &&
        lhs.BgG == rhs.BgG &&
        lhs.BgB == rhs.BgB 
    );
}

bool operator!=(const Cell& lhs, const Cell& rhs)
{
    return !operator==(lhs, rhs);
}