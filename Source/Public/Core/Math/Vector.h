#pragma once


#include "Core/Math/Vector2.h"


namespace Math
{
    template<typename T>
    struct TVector
    {
        union 
        {
            struct
            {
                T x;
                T y;
                T z;
            };
            float data[3];
        };
        TVector() = default;
        constexpr explicit inline TVector(T value);
        constexpr inline TVector(T inX, T inY, T inZ);
        //explicit inline TVector(const TVector2<T> vec, T inZ); 
    }; 

    template<typename T>
    constexpr inline TVector<T>::TVector(T value)
    : x(value)
    , y(value)
    , z(value)
    {}

    template<typename T>
    constexpr inline TVector<T>::TVector(T inX, T inY, T inZ)
    : x(inX)
    , y(inY)
    , z(inZ)
    {}

} // namespace Math
