#pragma once
#include "Core/Core.h"
#include "Core/Math/TypesForward.h"

namespace Math
{
    template<typename T>
    struct alignas(8) TVector2
    {
        union
        {
            struct 
            {
                T x;
                T y;
            };
            T data[2];  
        };
        TVector2() = default;
        constexpr explicit inline TVector2(T value);
        constexpr inline TVector2(T inX, T inY);
    };

    template<typename T>
    constexpr inline TVector2<T>::TVector2(T value)
    : x(value)
    , y(value)
    {}

    template<typename T>
    constexpr inline TVector2<T>::TVector2(T inX, T inY)
    : x(inX)
    , y(inY)
    {}
}