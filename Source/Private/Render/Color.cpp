#include "Render/Color.h"
#include "Render/Cell.h"

static uint64 EuclideanDistanceSq(int32 r1, int32 g1, int32 b1, int32 r2, int32 g2, int32 b2)
{
    int64 dr = r1 - r2;
    int64 dg = g1 - g2;
    int64 db = b1 - b2;
    return (dr * dr + dg * dg + db * db);
}


namespace Color
{
    uint8 ColorComponentToBinary(uint8 value)
    {
        return (value > 127) ? 1 : 0;
    }

    int32 RGBToAnsiCode(uint8 r, uint8 g, uint8 b) 
    {
        uint64 minDistanceSq = -1;
        int32 closestCode = 7;

        for (int32 i = static_cast<uint8>(EColorAnsi16::Black); i < static_cast<uint8>(EColorAnsi16::ReallyWhite); ++i)
        {
            Vector col = (+static_cast<EColorAnsi16>(i));
            uint64 distSq = EuclideanDistanceSq(r,g,b, col.x, col.y, col.z);

            if (distSq < minDistanceSq)
            {
                minDistanceSq = distSq;
                closestCode = i;
            }
        }
        return closestCode;
    }

    int32 MapBaseCodeToSGR(uint8 baseCode, bool bBackground)
    {
        int32 offset;

        if (baseCode < 8)
        {
            offset = bBackground ? 40 : 30;
        }
        else
        {
            offset = bBackground? 100 : 90;
            baseCode -= 8;
        }
        return offset + baseCode;
    }
    
} // namespace Color

