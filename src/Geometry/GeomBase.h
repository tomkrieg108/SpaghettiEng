#pragma once

#include "MathLib/MathLib.h"

namespace Geom
{
    using Point2d = SpgMth::Point2d;
    using Point3d = SpgMth::Point3d;

    enum class RelativePos 
    {
      Left, Right, Beyond, Behind, Between, Origin, Destination
    };

    inline bool Xor(bool x, bool y) 
    {
      return x ^ y;
    }

    constexpr float Epsilon(const float scale_factor = 100.0f)
    {
      return std::numeric_limits<float>::epsilon() * scale_factor;
    }
      
    inline bool Equal(float v1, float v2, const float scale_factor = 100.0f)
    {
      return SpgMth::NumUtils::Equal(v1,v2);
    }

    inline bool Equal(double v1, double v2)
    {
      return SpgMth::NumUtils::Equal(v1,v2);
    }

    inline bool Equal(const Point2d& a, const Point2d& b, const float scale_factor = 1000.0f)
    {
      return SpgMth::NumUtils::Equal(a,b);
    }
}