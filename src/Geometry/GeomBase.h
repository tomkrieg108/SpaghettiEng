#pragma once

#include <limits>

#include "MathLib/MathLib.h"

// {} []

namespace Geom
{
  using Real = float;

  using Point2d = glm::vec2;
  using Point3d = glm::vec3;
  using Point4d = glm::vec4;

  // ==== From GeomBase.h ========================================
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


  