#pragma once
#include "Core/Core.h"
#include "Core/Math/TypesForward.h"

namespace Math
{
    template<typename T>
    struct alignas(16) TVector4
    {
        union 
        {
            struct
            {
                T x;
                T y;
                T z;
                T w;
            };
            T data[4];
        };
        
    public:
        TVector4(const Math::TVector<T>& vec);
        explicit TVector4(T inX = 0.0f, T inY = 0.0f, T inZ = 0.0f, T inW = 1.0f);
        //explicit TVector4(TVector2<T> inXY, TVector2<T> inZW);

    };

    template<typename T>
    inline TVector4<T>::TVector4(const Math::TVector<T>& vec)
        : x(vec.x)
        , y(vec.y)
        , z(vec.z)
        , w(vec.w)
    {}

    template<typename T>
    inline TVector4<T>::TVector4(T inX, T inY, T inZ, T inW)
        : x(inX)
        , y(inY)
        , z(inZ)
        , w(inW)
    {}

} // namespace Math
