#pragma once
#include "Core/Core.h"

namespace Math
{
    template<typename T> struct TVector;
    template<typename T> struct TVector2;
    template<typename T> struct TVector4;

} // namespace Math

using Vector = Math::TVector<double>;
using Vector2 = Math::TVector2<double>;
using Vector4 = Math::TVector4<double>;

using iVector = Math::TVector<int32>;
using iVector2 = Math::TVector2<int32>;
using iVector4 = Math::TVector4<int32>;