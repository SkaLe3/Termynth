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
            T data[3];
        };
        TVector() = default;
        constexpr explicit inline TVector(T value);
        constexpr inline TVector(T inX, T inY, T inZ);
        explicit inline TVector(const TVector2<T> vec, T inZ); 

        template<typename U>
        explicit inline TVector(const TVector<U>& vec);


        [[nodiscard]] inline TVector<T> operator+(const TVector<T>& v) const;
        [[nodiscard]] inline TVector<T> operator-(const TVector<T>& v) const;
        [[nodiscard]] inline TVector<T> operator*(T scalar) const;
        [[nodiscard]] inline TVector<T> operator*(const TVector<T>& v) const;
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

    template <typename T>
    inline TVector<T>::TVector(const TVector2<T> vec, T inZ)
    : x(vec.x)
    , y(vec.y)
    , z(inZ)
    {}

    template <typename T>
    inline TVector<T> TVector<T>::operator+(const TVector<T> &v) const
    {
        return TVector<T>(x + v.x, y + v.y, z + v.z);
    }

    template <typename T>
    inline TVector<T> TVector<T>::operator-(const TVector<T> &v) const
    {
        return TVector<T>(x - v.x, y - v.y, z - v.z);
    }

    template <typename T>
    inline TVector<T> TVector<T>::operator*(T scalar) const
    {
        return TVector<T>(x * scalar, y * scalar, z * scalar);
    }

    template <typename T>
    inline TVector<T> TVector<T>::operator*(const TVector<T> &v) const
    {
        return TVector<T>(x * v.x, y * v.y, z * v.z);
    }

    template <typename T>
    template <typename U>
    inline TVector<T>::TVector(const TVector<U> &vec)
    : x(vec.x)
    , y(vec.y)
    , z(vec.z)
    {
    }

} // namespace Math
