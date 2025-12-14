#pragma once
#include "Core/Core.h"
#include "Core/Math/TypesForward.h"
#include "Core/Math/Func.h"

#include <cmath>

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

		template<typename U>
		explicit inline TVector2(const TVector2<U>& vec);

        [[nodiscard]] inline TVector2<T> operator+(const TVector2<T>& v) const;
        [[nodiscard]] inline TVector2<T> operator-(const TVector2<T>& v) const;
        [[nodiscard]] inline TVector2<T> operator*(T scalar) const;
        [[nodiscard]] inline TVector2<T> operator*(const TVector2<T>& v) const;

        [[nodiscard]] TVector2<T> GetAbs() const;
        [[nodiscard]] TVector2<T> GetNormal() const;
        [[nodiscard]] TVector2<T> Reflect(const TVector2<T>& normal) const;
        [[nodiscard]] inline static T Dot(const TVector2<T>& a, const TVector2<T>& b);

        [[nodiscard]] T Size() const;

    };
    template<typename T>
	template<typename U>
	inline TVector2<T>::TVector2(const TVector2<U>& vec)
        : x(vec.x)
        , y(vec.y)
	{

	}

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

    template <typename T>
    inline TVector2<T> TVector2<T>::operator+(const TVector2<T> &v) const
    {
        return TVector2<T>(x + v.x, y + v.y);
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator-(const TVector2<T> &v) const
    {
        return TVector2<T>(x - v.x, y - v.y);
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator*(T scalar) const
    {
        return TVector2<T>(x * scalar, y * scalar);
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator*(const TVector2<T> &v) const
    {
        return TVector2<T>(x * v.x, y * v.y);
    }
    template <typename T>
    inline TVector2<T> TVector2<T>::GetAbs() const
    {
        return TVector2<T>(std::abs(x), std::abs(y));
    }
    template <typename T>
    inline TVector2<T> TVector2<T>::GetNormal() const
    {
        const T ss = x * x + y * y;
        if (ss > 0)
        {
            const T scalar = InvSqrt(ss);
            return TVector2<T>(x*scalar, y*scalar);
        }
        return TVector2<T>(0.f, 0.f);
    }
    template <typename T>
    inline TVector2<T> TVector2<T>::Reflect(const TVector2<T> &normal) const
    {
        TVector2<T> unitNormal = normal.GetNormal();
        T vDotN = Dot(*this, unitNormal);
        T scaleFactor = 2.0f * vDotN;
        TVector2<T> projection = unitNormal * scaleFactor;
        TVector2<T> reflectedVector = *this - projection;
        return reflectedVector;
    }
    template <typename T>
    inline T TVector2<T>::Dot(const TVector2<T> &a, const TVector2<T> &b)
    {
        return a.x * b.x + a.y * b.y;
    }
    template <typename T>
    inline T TVector2<T>::Size() const
    {
        return x*x + y*y;
    }
}