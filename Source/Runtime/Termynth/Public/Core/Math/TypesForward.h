#pragma once
#include "Core/Core.h"

namespace Math
{
    template<typename T> class TVector;
    template<typename T> class TVector2;
    template<typename T> class TVector4;

} // namespace Math

using Vector = Math::TVector<double>;
using Vector2 = Math::TVector2<double>;
using Vector4 = Math::TVector4<double>;

using iVector = Math::TVector<int32>;
using iVector2 = Math::TVector2<int32>;
using iVector4 = Math::TVector4<int32>;