#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp> //epsilonEqual()

namespace Geom
{
  using Point2d = glm::vec2;

  enum class RelativePos 
  {
    Left, Right, Beyond, Behind, Between, Origin, Destination
  };

  inline bool Xor(bool x, bool y) {
	  return x ^ y;
  }

  constexpr float Epsilon(const float scale_factor = 100.0f)
  {
    return std::numeric_limits<float>::epsilon() * scale_factor;
  }

  inline bool Equal(float v1, float v2) //link error if not inline
  {
    return std::fabs(v1 - v2) < Epsilon();
  }

  inline bool Equal(const Point2d& a, const Point2d& b)
  {
    bool allEqual = glm::all(glm::epsilonEqual(a, b, Epsilon()));
    return allEqual;
  }
}